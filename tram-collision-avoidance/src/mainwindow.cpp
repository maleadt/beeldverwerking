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
#include <QFileDialog>
#include <QDebug>


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
    mUI->videoLayout->addWidget(mGLWidget);
    mUI->actionStart->setEnabled(false);
    mUI->actionStop->setEnabled(false);

    // Exit action
    mActionExit = new QAction(tr("E&xit"), this);
    mActionExit->setShortcut(tr("Ctrl+Q"));
    mActionExit->setStatusTip(tr("Exit the application"));
    connect(mActionExit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    // Recent file actions
    for (int i = 0; i < MaxRecentFiles; ++i) {
        mActionsRecentFiles[i] = new QAction(this);
        mActionsRecentFiles[i]->setVisible(false);
        connect(mActionsRecentFiles[i], SIGNAL(triggered()),
                this, SLOT(on_actionRecentFile_triggered()));
    }

    // Build recent files
    QMenu* fileMenu = mUI->menuFile;
    mActionSeparator = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(mActionsRecentFiles[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(mActionExit);
    updateRecentFileActions();
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

void MainWindow::on_actionStart_triggered()
{
    // Statusbar message
    mUI->statusBar->showMessage("Starting processing...");
    mUI->actionStart->setEnabled(false);

    // Schedule processing
    mUI->actionStop->setEnabled(true);
    mProcessing = true;
    process();
}

void MainWindow::on_actionStop_triggered()
{
    // Statusbar message
    mUI->statusBar->showMessage("Stopped processing");
    mProcessing = false;
    mUI->actionStart->setEnabled(true);
    mUI->actionStop->setEnabled(false);
}

void MainWindow::on_actionOpen_triggered()
{
    QString tFilename = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.avi *.mp4)"));
    openFile(tFilename);
}

void MainWindow::on_actionRecentFile_triggered()
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
    if (mVideoCapture != 0)
    {
        // Close and delete the capturer
        if (mVideoCapture->isOpened())
            mVideoCapture->release();
        delete mVideoCapture;
        mVideoCapture = 0;

        // Update the interface
        mUI->actionStart->setEnabled(false);
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

    // Open output video
#if WRITE_VIDEO
    std::string oVideoFile = argv[2];
    mVideoWriter = new cv::VideoWriter(oVideoFile,
                             CV_FOURCC('M', 'J', 'P', 'G'),
                             mVideoCapture->get(CV_CAP_PROP_FPS),
                             cv::Size(mVideoCapture->get(CV_CAP_PROP_FRAME_WIDTH),mVideoCapture->get(CV_CAP_PROP_FRAME_HEIGHT)),
                             true);
#endif

    statusBar()->showMessage("File opened and loaded");
    mUI->actionStart->setEnabled(true);
    setTitle(iFilename);
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
            cv::Mat tOutput = processFrame(tFrame);
            mGLWidget->sendImage(&tOutput);
            QTimer::singleShot(25, this, SLOT(process()));
        }
    }
}

cv::Mat MainWindow::processFrame(cv::Mat &iFrame)
{
    // Initialisation
    std::cout << "-- PROCESSING FRAME -- " << std::endl;

    // Load objects
    TrackDetection tTrackDetection(&iFrame);
    TramDetection tTramDetection(&iFrame);

    // Preprocess
    std::cout << "* Preprocessing" << std::endl;
    tTrackDetection.preprocess();
    tTramDetection.preprocess();

    // Find features
    std::cout << "* Finding features" << std::endl;
    try
    {
        tTrackDetection.find_features(mFeatures);
    }
    catch (FeatureException e)
    {
        std::cout << "  Error finding tracks: " << e.what() << std::endl;
    }
    try
    {
        tTramDetection.find_features(mFeatures);
    }
    catch (FeatureException e)
    {
        std::cout << "  Error finding tram: " << e.what() << std::endl;
    }

    // Draw image
    std::cout << "* Drawing image" << std::endl;
    cv::Mat tVisualisation;
    if (mUI->visualisationType->currentIndex() == 0)
        tVisualisation = iFrame.clone();
    else if (mUI->visualisationType->currentIndex() == 1)
        tVisualisation = tTrackDetection.frameDebug();
    else if (mUI->visualisationType->currentIndex() == 2)
        tVisualisation = tTramDetection.frameDebug();
    if (mUI->drawFeatures->isChecked())
    {
        // Draw tracks
        if (mFeatures.track_left.size())
        {
            for (size_t i = 0; i < mFeatures.track_left.size()-1; i++)
                cv::line(tVisualisation, mFeatures.track_left[i], mFeatures.track_left[i+1], cv::Scalar(0, 255, 0), 3);
        }
        if (mFeatures.track_right.size())
        {
            for (size_t i = 0; i < mFeatures.track_right.size()-1; i++)
                cv::line(tVisualisation, mFeatures.track_right[i], mFeatures.track_right[i+1], cv::Scalar(0, 255, 0), 3);
        }

        // Draw tram
        cv::rectangle(tVisualisation, mFeatures.tram, cv::Scalar(0, 255, 0), 1);
    }

    return tVisualisation;
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
