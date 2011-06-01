#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <cmath>
#include <iostream>

using namespace cv;

int main(int, char**)
{
    //VideoCapture cap("/home/tim/Desktop/obstakeldetectie.avi");
    VideoCapture cap("/home/sebastiaan/beeldverwerking/obstakeldetectie.avi");
    if(!cap.isOpened())
        return -1;

    Mat edges, edges_color;
    namedWindow("edges", 1);
    for(;;)
    {
        Mat frame;
        cap >> frame;
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 20, 50, 3);
        cvtColor(edges, edges_color, CV_GRAY2BGR );

        vector<Vec4i> lines;
        HoughLinesP(edges,      // Image
                    lines,      // Lines
                    1,          // Rho
                    CV_PI/180,  // Theta
                    15,         // Threshold
                    150,        // Minimum line length
                    5           // Maximum line gap
                    );
        for( size_t i = 0; i < lines.size(); i++ )
        {
            double dy = double(lines[i][3] - lines[i][1]);
            double dx = double(lines[i][2] - lines[i][0]);
            double rc = abs(dy/dx);
         //   std::cout << "rc: " << rc << std::endl;
            line(
                        edges_color,
                        Point(lines[i][0],
                              lines[i][1]),
                        Point(lines[i][2],
                              lines[i][3]),
                        Scalar(0,0,255),
                        3,
                        8
             );
        }

        imshow("edges", edges_color);
        if(waitKey(30) >= 0)
            break;
    }

    return 0;
}
