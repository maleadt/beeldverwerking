//
// Configuration
//

// Include guard
#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

// Includes
#include <QGLWidget>
#include <QImage>
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"

class OpenGLRenderer : public QGLWidget
{
    Q_OBJECT
public:
    // Construction and destruction
    OpenGLRenderer(QWidget* parent = 0);

    // Getters and setters
    void setImage(IplImage *iplImage);

    // GL functions
    void paintGL();
    void initializeGL();
    void resizeGL(int iWidth, int iHeight);

private:
    IplImage* mImage;
};

#endif // OPENGLRENDERER_H
