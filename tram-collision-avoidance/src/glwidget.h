//
// Configuration
//

// Include guard
#ifndef GLWIDGET_H
#define GLWIDGET_H

// Includes
#include <QGLWidget>
#include <QtOpenGL>
#include <cv.h>
#include <highgui.h>


class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    // Construction and destruction
    GLWidget();

    // Image loading
    void sendImage(cv::Mat *img);

protected:
    // OpenGL functionality
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
private:
    QImage qframe;
};

#endif // GLWIDGET_H
