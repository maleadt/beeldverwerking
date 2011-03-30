////////////////////////////////////////////////////////////////////////
//
// hello-world.cpp
//
// This is a simple, introductory OpenCV program. The program reads an
// image from a file, inverts it, and displays the result.
//
////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "opencv/cv.h"
#include "opencv/highgui.h""


int main(int argc, char *argv[])
{
    cvNamedWindow( "Example2", CV_WINDOW_AUTOSIZE );
    CvCapture* capture = cvCreateFileCapture( "/home/ruben/beeldverwerking/out.avi" );
    IplImage* frame;
    while(1) {
            frame = cvQueryFrame( capture );
            if( !frame ) break;
            cvShowImage( "Example2", frame );
            char c = cvWaitKey(33);
            if( c == 27 ) break;
    }
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Example2" );
    return 0;
}
