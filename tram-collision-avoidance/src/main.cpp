///////////////////////////////////////////////////////////////////////////////
// Configuration
//

// Headers
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <iostream>
#include "framefeatures.h"
#include "component.h"
#include "trackdetection.h"
#include "pedestriandetection.h"
#include "vehicledetection.h"



///////////////////////////////////////////////////////////////////////////////
// Main
//
template <typename T>
        std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}
int main(int argc, char** argv)
{


    //string filename = "/home/ruben/thuis/"; int total = 3; string ext = ".jpg";
    string filename = "/home/ruben/wheels/1/"; int total = 5; string ext = ".png";
    //string filename = "/home/ruben/wheels/"; int total = 8; string ext = ".png";
    //string filename = "/home/ruben/pedestrians/"; int total = 14; string ext = ".png";

    // Setup OpenCV
    cv::namedWindow("tram collision avoidance", 1);


    for (int i = 1; i <= total; i++) {
        cv::Mat image = cv::imread(filename+to_string(i)+ext, 1);

        FrameFeatures tFeatures;

        PedestrianDetection pD(image);
        pD.preprocess();
        pD.find_features(tFeatures);

        VehicleDetection vD(image);
        vD.preprocess();
        vD.find_features(tFeatures);

        std::cout<<"Found pedestrians: "<<tFeatures.pedestrians.size()<<std::endl;
        std::cout<<"Found vehicles: "<<tFeatures.vehicles.size()<<std::endl;


        for (int i = 0; i < tFeatures.pedestrians.size(); i++) {
            cv::Rect r = tFeatures.pedestrians[i];
            cv::rectangle(image, r.tl(), r.br(), cv::Scalar(0,0,255), 2);
        }
        for (int i = 0; i < tFeatures.vehicles.size(); i++) {
            cv::Rect r = tFeatures.vehicles[i];
            cv::rectangle(image, r.tl(), r.br(), cv::Scalar(0,255,0), 1);
        }

        cv::imshow("people detector", image);
        cv::waitKey();

    }


    return 0;
}

