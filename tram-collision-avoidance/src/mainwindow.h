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
    void on_actionRecentFile_triggered();

    // File and vidoe processing
private:
    void openFile(QString iFilename);
    void process();
    cv::Mat processFrame(cv::Mat& iFrame);

    // Auxiliary
private slots:
    void updateRecentFileActions();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

private:
    // Member data
    QString mVideoFilename;
    QTime mTimer;
    GLWidget* mGLWidget;
    cv::VideoCapture* mVideoCapture;
#if WRITE_VIDEO
    cv::VideoWriter* mVideoWriter;
#endif
    QSettings* mSettings;

    // UI members
    Ui::MainWindow* mUI;
    QAction *mActionSeparator;
    QAction *mActionExit;
    enum { MaxRecentFiles = 5 };
    QAction *mActionsRecentFiles[MaxRecentFiles];

    // Detection state
    FrameFeatures mFeatures;
};

#endif // MAINWINDOW_H
