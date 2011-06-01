//
// Configuration
//

// Includes
#include "openglrenderer.h"


//
// Construction and destruction
//

OpenGLRenderer::OpenGLRenderer(QWidget* parent): QGLWidget(parent)
{
    mImage = 0;

}


//
// Getters and setters
//ctions

void OpenGLRenderer::setImage(IplImage* _im)
{
    if(!_im)
        return;

    this->mImage = _im;

    this->paintGL();
    this->resizeGL(mImage->width,mImage->height);
    this->update();

}


//
// GL fun
//

void OpenGLRenderer::resizeGL(int iWidth, int iHeight)
{
    if(!mImage)
        return;

    // Adjust the viewport
    glViewport(0, 0, mImage->width, mImage->height);

    //,Adjust the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-mImage->width/2, mImage->width/2, mImage->height/2, -mImage->height/2, -1, 1);
    //QGLWidget::resizeGL(w,h);

}

void OpenGLRenderer::paintGL()
{
    if(!mImage)
        return;

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor (1.0,1.0,1.0,1.0);
    //Set the raster position
    /*
      The position seems to be the inverse because the rendering is
      affected by the glPixelZoom call.
     */
    glRasterPos2i(-mImage->width/2, -mImage->height/2);

    // Invert the image (the data coming from OpenCV is inverted)
    glPixelZoom(1.0f, -1.0f);

    // Draw image
    glDrawPixels(mImage->width, mImage->height, GL_BGR, GL_UNSIGNED_BYTE, mImage->imageData);

    // Update widget
    this->update();
}

void OpenGLRenderer::initializeGL()
{
    if(!mImage)
        return;

    // Adjust the viewport
    glViewport(0,0,mImage->width, mImage->height);

    // Adjust the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-mImage->width/2, mImage->width/2, mImage->height/2, -mImage->height/2, -1, 1);

    this->paintGL();
    this->update();
}
