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
    // Setup interface
    mUI->setupUi(this);
    mGLWidget = new GLWidget();
    mUI->verticalLayout->addWidget(mGLWidget);

    mUI->actionStart->setEnabled(false);
    mUI->actionStop->setEnabled(false);
}

MainWindow::~MainWindow()
{
    if (mVideoCapture->isOpened())
        mVideoCapture->release();
    delete mVideoCapture;
    delete mUI;
}


//
// UI slots
//

void MainWindow::on_actionStart_triggered()
{
    // Statusbar message
    mUI->statusBar->showMessage("Started processing");
    mUI->actionStart->setEnabled(false);

    // Open input video
    mVideoCapture = new cv::VideoCapture(mVideoFilename.toStdString());
    if(!mVideoCapture->isOpened())
    {
        std::cout << "Error: could not 'open video" << std::endl;
        return;
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

    // Setup variables
    mFrameCount = 0;
    mOldFeatures;
    mAgeTrack = 0;
    mVisualisationType = FINAL;
    mVisualisationDuration = 0;

    // Schedule processing
    mUI->actionStop->setEnabled(true);
    process();
}

void MainWindow::on_actionStop_triggered()
{
    // Statusbar message
    mUI->statusBar->showMessage("Stopped processing");
    mVideoCapture->release();
    mUI->actionStart->setEnabled(true);
    mUI->actionStop->setEnabled(false);
}

void MainWindow::on_actionOpen_triggered()
{
    mVideoFilename = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.avi *.mp4)"));
    if (QFileInfo(mVideoFilename).isReadable())
    {
        mUI->actionStart->setEnabled(true);
    }
    else
    {
        qWarning() << "File '" << mVideoFilename << "' not readable";
        mUI->actionStart->setEnabled(false);
    }
}


//
// Auxiliary
//

void MainWindow::process()
{
    if (mVideoCapture->isOpened())
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
    std::cout << "-- PROCESSING FRAME " << mFrameCount++ << " --" << std::endl;
    FrameFeatures tFeatures;

    // Manage visualisation
    cv::Mat tVisualisation;
    if (mVisualisationType == FINAL)
        tVisualisation = iFrame.clone();

    // Load objects
    TrackDetection tTrackDetection(iFrame);
    TramDetection tTramDetection(iFrame);

    // Preprocess
    std::cout << "* Preprocessing" << std::endl;
    tTrackDetection.preprocess();
    tTramDetection.preprocess();
    if (mVisualisationType == DEBUG_TRACK)
        tVisualisation = tTrackDetection.frameDebug();
    else if (mVisualisationType == DEBUG_TRAM)
        tVisualisation = tTramDetection.frameDebug();

    // Find features
    std::cout << "* Finding features" << std::endl;
    try
    {
        // Find tracks
        std::cout << "- Finding tracks" << std::endl;
        try
        {
            tTrackDetection.find_features(tFeatures);
            tTrackDetection.copy_features(tFeatures, mOldFeatures);
            mAgeTrack = FEATURE_EXPIRATION;
        }
        catch (FeatureException e)
        {
            std::cout << "  Warning: " << e.what() << std::endl;
            if (mAgeTrack > 0)
            {
                mAgeTrack--;
                tTrackDetection.copy_features(mOldFeatures, tFeatures);
            }
            else
                throw FeatureException("could not find the tracks");
        }

        // Draw tracks
        if (mVisualisationType == FINAL)
        {
            for (size_t i = 0; i < tFeatures.track_left.size()-1; i++)
                cv::line(tVisualisation, tFeatures.track_left[i], tFeatures.track_left[i+1], cv::Scalar(0, 255, 0), 3);
            for (size_t i = 0; i < tFeatures.track_right.size()-1; i++)
                cv::line(tVisualisation, tFeatures.track_right[i], tFeatures.track_right[i+1], cv::Scalar(0, 255, 0), 3);
        }

        // Find tracks
        std::cout << "- Finding tracks" << std::endl;
        try
        {
            tTramDetection.find_features(tFeatures);
        }
        catch (FeatureException e)
        {
        }

        // Draw tram
        if (mVisualisationType == FINAL)
        {
            cv::rectangle(tVisualisation, tFeatures.tram, cv::Scalar(0, 255, 0), 1);
        }
    }
    catch (FeatureException e)
    {
        std::cout << "! Error: " << e.what() << std::endl;
    }

    return tVisualisation;
}
