//
// Configuration
//

// Include guard
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Includes
#include <QtGui/QMainWindow>
#include "cv.h"
#include "highgui.h"
#include "glwidget.h"
#include "ui_mainwindow.h"
#include <QTime>
#include "framefeatures.h"
#include "featureexception.h"

// Definitions
#define FEATURE_EXPIRATION 5
#define WRITE_VIDEO 0

// Enumerations
enum Visualisation {
    FINAL = 1,
    DEBUG_TRACK,
    DEBUG_TRAM
};
template <class Enum> Enum & enum_increment(Enum& value, Enum begin, Enum end)
{
    return value = (value == end) ? begin : Enum(value + 1);
}

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    // Construction and destructionOpencv2Qt
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    // UI slots
private slots:
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionOpen_triggered();

    // Auxiliary
private slots:
    void process();
    cv::Mat processFrame(cv::Mat& iFrame);

private:
    // Member data
    QString mVideoFilename;
    QTime mTimer;
    Ui::MainWindow* mUI;
    GLWidget* mGLWidget;
    cv::VideoCapture* mVideoCapture;
#if WRITE_VIDEO
    cv::VideoWriter* mVideoWriter;
#endif

    // Detection state
    unsigned int mFrameCount;
    FrameFeatures mOldFeatures;
    unsigned int mAgeTrack;
    Visualisation mVisualisationType;
    unsigned int mVisualisationDuration;
};

#endif // MAINWINDOW_H
