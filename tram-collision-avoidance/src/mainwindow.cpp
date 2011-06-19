//
// Configuration
//

// Includes
#include "mainwindow.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <iostream>
#include "trackdetection.h"
#include "tramdetection.h"
#include "tramdistance.h"
#include "pedestriandetection.h"
#include "vehicledetection.h"
#include <QFileDialog>
#include <QDebug>
#ifdef _OPENMP
#include <omp.h>
#endif

// Definitions
#define FEATURES_MAX_AGE 10


//
// Construction and destruction
//

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), mUI(new Ui::MainWindow)
{
    // Initialize application
    mSettings = new QSettings("Beeldverwerking", "Tram Collision Detection");
    mVideoCapture = 0;
#if WRITE_VIDEO
    mVideoWriter = 0;
#endif

    // Setup interface
    mUI->setupUi(this);
    mGLWidget = new GLWidget();
    mUI->lytVideo->addWidget(mGLWidget);
    mUI->btnStart->setEnabled(false);
    mUI->btnStop->setEnabled(false);

    // Exit action
    mActionExit = new QAction(tr("E&xit"), this);
    mActionExit->setShortcut(tr("Ctrl+Q"));
    mActionExit->setStatusTip(tr("Exit the application"));
    connect(mActionExit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    // Recent file actions
    for (int i = 0; i < MaxRecentFiles; ++i) {
        mActionsRecentFiles[i] = new QAction(this);
        mActionsRecentFiles[i]->setVisible(false);
        connect(mActionsRecentFiles[i], SIGNAL(triggered()), this, SLOT(on_actRecentFile_triggered()));
    }

    // Build recent files
    QMenu* fileMenu = mUI->menuFile;
    mActionSeparator = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(mActionsRecentFiles[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(mActionExit);
    updateRecentFileActions();

    // Print a message
#ifdef _OPENMP
    mUI->statusBar->showMessage("Application initialized (multithreaded execution, using up to " + QString::number(omp_get_max_threads()) + " core(s)");
#else
    mUI->statusBar->showMessage("Application initialized (singelthreaded execution)");
#endif
    mFrameCounter = 0; drawStats();
    setTitle();
}

MainWindow::~MainWindow()
{
    if (mVideoCapture != 0)
    {
        if (mVideoCapture->isOpened())
            mVideoCapture->release();
        delete mVideoCapture;
    }

#if WRITE_VIDEO
    if (mVideoWriter != 0)
    {
        if (mVideoWriter->isOpened())
            mVideoWriter->release();
        delete mVideoWriter;
    }
#endif

    delete mUI;
}


//
// UI slots
//

void MainWindow::on_btnStart_clicked()
{
    // Statusbar message
    mUI->statusBar->showMessage("Starting processing...");
    mUI->btnStart->setEnabled(false);

    // Schedule processing
    mUI->btnStop->setEnabled(true);
    mUI->btnStop->setFocus();
    mProcessing = true;
    process();
}

void MainWindow::on_btnStop_clicked()
{
    // Statusbar message
    mUI->statusBar->showMessage("Stopped processing");
    mProcessing = false;
    mUI->btnStart->setEnabled(true);
    mUI->btnStart->setFocus();
    mUI->btnStop->setEnabled(false);
}

void MainWindow::on_actOpen_triggered()
{
    QString tFilename = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.avi *.mp4)"));
    openFile(tFilename);
    mUI->btnStart->setEnabled(true);
}

void MainWindow::on_actRecentFile_triggered()
{
    QAction *tAction = qobject_cast<QAction *>(sender());
    if (tAction)
        openFile(tAction->data().toString());
}


//
// File and video processing
//

bool MainWindow::openFile(QString iFilename)
{
    // Do we need to clean up a previous file?
    mProcessing = false;
    if (mVideoCapture != 0)
    {
        // Close and delete the capturer
        if (mVideoCapture->isOpened())
            mVideoCapture->release();
        delete mVideoCapture;
        mVideoCapture = 0;

        // Update the interface
        mUI->btnStart->setEnabled(false);
        mFrameCounter = 0; drawStats();
        setTitle();
    }

    // Check if we can open the file
    if (! QFileInfo(iFilename).isReadable())
    {
        statusBar()->showMessage("Error: could not read file");
        return false;
    }

    // Open input video
    mVideoCapture = new cv::VideoCapture(iFilename.toStdString());
    if(!mVideoCapture->isOpened())
    {
        statusBar()->showMessage("Error: could not open file");
        return false;
    }
    //mGLWidget->setMinimumSize(mVideoCapture->get(CV_CAP_PROP_FRAME_WIDTH), mVideoCapture->get(CV_CAP_PROP_FRAME_HEIGHT));

    // Open output video
#if WRITE_VIDEO
    std::string oVideoFile = argv[2];
    mVideoWriter = new cv::VideoWriter(oVideoFile,
                                       CV_FOURCC('M', 'J', 'P', 'G'),
                                       mVideoCapture->get(CV_CAP_PROP_FPS),
                                       cv::Size(mVideoCapture->get(CV_CAP_PROP_FRAME_WIDTH), mVideoCapture->get(CV_CAP_PROP_FRAME_HEIGHT)),
                                       true);
#endif

    // Reset time counters
    mFrameCounter = 0;
    mTimePreprocess = 0;
    mTimeTrack = 0;
    mTimeTram = 0;
    mTimeDistance = 0;
    mTimePedestrians = 0;
    mTimeVehicle = 0;
    mTimeDraw = 0;

    // Reset age trackers
    mAgeTrack = 0;
    mAgeTram = 0;
    mAgePedestrian = 0;
    mAgeVehicle = 0;

    statusBar()->showMessage("File opened and loaded");
    mUI->btnStart->setEnabled(true);
    mUI->btnStop->setEnabled(false);
    setCurrentFile(iFilename);
    return true;
}

void MainWindow::process()
{
    if (mProcessing && mVideoCapture->isOpened())
    {
        mTimer.restart();
        cv::Mat tFrame;
        *mVideoCapture >> tFrame;
        if (tFrame.data)
        {
            processFrame(tFrame);
            mFrameCounter++;
            drawStats();
            QTimer::singleShot(25, this, SLOT(process()));
        }
    }
}

void MainWindow::processFrame(cv::Mat &iFrame)
{
    // Load objects
    TrackDetection tTrackDetection(&iFrame);
    TramDetection tTramDetection(&iFrame);
    TramDistance tTramDistance(&iFrame);
    PedestrianDetection tPedestrianDetection(&iFrame);
    VehicleDetection tVehicleDetection(&iFrame);

    // Preprocess
    timeStart();
#pragma omp parallel sections
    {
#pragma omp section
        {
            tTrackDetection.preprocess();
        }
#pragma omp section
        {
            tTramDetection.preprocess();
        }
#pragma omp section
        {
            tPedestrianDetection.preprocess();
        }
#pragma omp section
        {
            tVehicleDetection.preprocess();
        }
    }
    mTimePreprocess += timeDelta();

    // Find features
    timeStart();
    try
    {
        tTrackDetection.find_features(mFeatures);
        mAgeTrack = mFrameCounter;
    }
    catch (FeatureException e)
    {
        std::cout << "  Error finding tracks: " << e.what() << std::endl;
    }
    mTimeTrack += timeDelta();
    try
    {
        tTramDetection.find_features(mFeatures);
        mAgeTram = mFrameCounter;
    }
    catch (FeatureException e)
    {
        std::cout << "  Error finding tram: " << e.what() << std::endl;
    }
    mTimeTram += timeDelta();
   try
    {
        tTramDistance.find_features(mFeatures);
        mAgeTram = mFrameCounter;
    }
    catch (FeatureException e)
    {
        std::cout << "  Error finding distance: " << e.what() << std::endl;
    }
    mTimeDistance += timeDelta();

    try
    {
        //tPedestrianDetection.find_features(mFeatures);
        mAgePedestrian = mFrameCounter;
    }
    catch (FeatureException e)
    {
        std::cout << "  Error finding pedestrians: " << e.what() << std::endl;
    }
    mTimePedestrians += timeDelta();

    try
    {
        //tVehicleDetection.find_features(mFeatures);
        mAgeVehicle = mFrameCounter;
    }
    catch (FeatureException e)
    {
        std::cout << "  Error finding vehicles: " << e.what() << std::endl;
    }
    mTimeVehicle += timeDelta();

    // Draw image
    timeStart();
    cv::Mat tVisualisation;
    if (mUI->slcType->currentIndex() == 0)
        tVisualisation = iFrame.clone();
    else if (mUI->slcType->currentIndex() == 1)
        tVisualisation = tTrackDetection.frameDebug();
    else if (mUI->slcType->currentIndex() == 2)
        tVisualisation = tTramDetection.frameDebug();
    else if (mUI->slcType->currentIndex() == 3)
        tVisualisation = tPedestrianDetection.frameDebug();
    else if (mUI->slcType->currentIndex() == 4)
        tVisualisation = tVehicleDetection.frameDebug();

    if (mUI->chkFeatures->isChecked())
    {
        // Draw tracks
        if (mFeatures.tracks.first.size())
        {
            for (int i = 0; i < mFeatures.tracks.first.size()-1; i++)
                cv::line(tVisualisation, mFeatures.tracks.first[i], mFeatures.tracks.first[i+1], cv::Scalar(0, 255, 0), 3);
        }
        if (mFeatures.tracks.second.size())
        {
            for (int i = 0; i < mFeatures.tracks.second.size()-1; i++)
                cv::line(tVisualisation, mFeatures.tracks.second[i], mFeatures.tracks.second[i+1], cv::Scalar(0, 255, 0), 3);
        }

        // Draw tram
        cv::rectangle(tVisualisation, mFeatures.tram, cv::Scalar(0, 255, 0), 1);

        // Draw line between middle of the tram and middle of the track
/*        std::cout << "From: " << mFeatures.trackHalfX.x << "," << mFeatures.trackHalfX.y  << std::endl;
        std::cout << "To: " << mFeatures.tramHalfX.x << "," << mFeatures.tramHalfX.y << std::endl;
        cv::line(tVisualisation, mFeatures.trackHalfX, mFeatures.tramHalfX, cv::Scalar(0, 255, 0), 3);
*/
        //Draw pedestrians
        for (size_t i = 0; i < mFeatures.pedestrians.size(); i++) {
            cv::Rect r = mFeatures.pedestrians[i];
            cv::rectangle(tVisualisation, r.tl(), r.br(), cv::Scalar(0,0,255), 2);
        }

        //Draw vehicles
        for (size_t i = 0; i < mFeatures.vehicles.size(); i++) {
            cv::Rect r = mFeatures.vehicles[i];
            cv::rectangle(tVisualisation, r.tl(), r.br(), cv::Scalar(255,0,0), 1);
        }
    }    
    mGLWidget->sendImage(&tVisualisation);
    mTimeDraw += timeDelta();

    // Check for outdated features
    if (mFrameCounter - mAgeTrack > FEATURES_MAX_AGE)
    {
        mFeatures.tracks.first.clear();
        mFeatures.tracks.second.clear();
    }
    if (mFrameCounter - mAgeTram > FEATURES_MAX_AGE)
        mFeatures.tram = cv::Rect();
    if (mFrameCounter - mAgePedestrian > FEATURES_MAX_AGE)
        mFeatures.pedestrians.clear();
    if (mFrameCounter - mAgeVehicle > FEATURES_MAX_AGE)
        mFeatures.vehicles.clear();
}

void MainWindow::drawStats()
{
    int mPreprocessDelta = 0, mTrackDelta = 0, mTramDelta = 0, mPedestriansDelta = 0, mVehicleDelta = 0, mTimeDelta = 0;
    if (mFrameCounter > 0)
    {
        mPreprocessDelta = mTimePreprocess / mFrameCounter;
        mTrackDelta = mTimeTrack / mFrameCounter;
        mTramDelta = mTimeTram / mFrameCounter;
        mPedestriansDelta = mTimePedestrians / mFrameCounter;
        mVehicleDelta = mTimeVehicle / mFrameCounter;
        mTimeDelta = mTimeDraw / mFrameCounter;
    }
    mUI->lblPreprocess->setText("Preprocess: " + QString::number(mPreprocessDelta) + " ms");
    mUI->lblTrack->setText("Track: " + QString::number(mTrackDelta) + " ms");
    mUI->lblTram->setText("Tram: " + QString::number(mTramDelta) + " ms");
    mUI->lblPedestrian->setText("Pedestrian: " + QString::number(mPedestriansDelta) + " ms");
    mUI->lblVehicle->setText("Vehicle: " + QString::number(mVehicleDelta) + " ms");
    mUI->lblDraw->setText("Draw: " + QString::number(mTimeDelta) + " ms");
}


//
// Auxiliary
//

void MainWindow::updateRecentFileActions()
{
    // Fetch the saved list
    QStringList tRecentFiles = mSettings->value("recentFileList").toStringList();
    int tRecentFileCount = qMin(tRecentFiles.size(), (int)MaxRecentFiles);

    // Update the action list
    for (int i = 0; i < tRecentFileCount; ++i)
    {
        QString tText = tr("&%1 %2").arg(i + 1).arg(strippedName(tRecentFiles[i]));
        mActionsRecentFiles[i]->setText(tText);
        mActionsRecentFiles[i]->setData(tRecentFiles[i]);
        mActionsRecentFiles[i]->setVisible(true);
    }
    for (int i = tRecentFileCount; i < MaxRecentFiles; ++i)
        mActionsRecentFiles[i]->setVisible(false);
    mActionSeparator->setVisible(tRecentFileCount > 0);
}

void MainWindow::setCurrentFile(const QString &iFilename)
{
    // Update the file list
    QStringList tFiles = mSettings->value("recentFileList").toStringList();
    tFiles.removeAll(iFilename);
    tFiles.prepend(iFilename);
    while (tFiles.size() > MaxRecentFiles)
        tFiles.removeLast();
    mSettings->setValue("recentFileList", tFiles);

    // Update the interface
    setTitle(iFilename);
    updateRecentFileActions();
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::setTitle(QString iFilename)
{
    if (iFilename.isEmpty())
        setWindowTitle(tr("Tram Collision Detection"));
    else
        setWindowTitle(tr("%1 - %2").arg(strippedName(iFilename))
                       .arg("Tram Collision Detection"));

}

void MainWindow::timeStart()
{
    mTime = QDateTime::currentMSecsSinceEpoch();
}

unsigned long MainWindow::timeDelta()
{
    unsigned long tCurrentTime = QDateTime::currentMSecsSinceEpoch();
    unsigned long tDelta = tCurrentTime - mTime;
    mTime = tCurrentTime;
    return tDelta;
}
