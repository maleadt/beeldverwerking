#ifndef VEHICLEDETECTION_H
#define VEHICLEDETECTION_H

// Includes

#include "opencv/cv.h"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <list>
#include <cmath>
#include <vector>
#include "component.h"
#include "framefeatures.h"

class VehicleDetection : public Component
{
public:
    // Construction and destruction
    VehicleDetection(cv::Mat const* iFrame);

    // Component interface
    void preprocess();
    void find_features(FrameFeatures& iFrameFeatures) throw(FeatureException);
    cv::Mat frameDebug() const;

private:
    // Feature detection
    void cropFrame();
    void detectWheels();
    void detectVehiclesFromWheels(FrameFeatures& iFrameFeatures);
    std::vector<cv::Rect> vehicles;
    int tracksWidth, tracksStartCol, tracksEndCol;
    int adjustedX;

    cv::Mat mFrameCropped;

    // Frames
    cv::Mat mFramePreprocessed;
    cv::Mat mFrameDebug;
};

#endif // VEHICLEDETECTION_H
