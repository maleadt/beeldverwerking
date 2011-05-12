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
    mUI->videoLayout->addWidget(mGLWidget);
    mUI->buttonStart->setEnabled(false);
    mUI->buttonStop->setEnabled(false);
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

void MainWindow::on_buttonStart_clicked()
{
    // Statusbar message
    mUI->statusBar->showMessage("Started processing");
    mUI->buttonStart->setEnabled(false);

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

    // Schedule processing
    mUI->buttonStop->setEnabled(true);
    process();
}

void MainWindow::on_buttonStop_clicked()
{
    // Statusbar message
    mUI->statusBar->showMessage("Stopped processing");
    mVideoCapture->release();
    mUI->buttonStart->setEnabled(true);
    mUI->buttonStop->setEnabled(false);
}

void MainWindow::on_actionOpen_triggered()
{
    mVideoFilename = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.avi *.mp4)"));
    if (QFileInfo(mVideoFilename).isReadable())
    {
        mUI->buttonStart->setEnabled(true);
    }
    else
    {
        qWarning() << "File '" << mVideoFilename << "' not readable";
        mUI->buttonStart->setEnabled(false);
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
