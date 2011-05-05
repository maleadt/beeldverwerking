#ifndef VEHICLEDETECTION_H
#define VEHICLEDETECTION_H

// Includes

#include "opencv/cv.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

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
    VehicleDetection(const cv::Mat& iFrame);

    // Component interface
    void preprocess();
    void find_features(FrameFeatures& iFrameFeatures) throw(FeatureException);
    void copy_features(const FrameFeatures& from, FrameFeatures& to) const;
    cv::Mat frameDebug() const;

private:
    // Feature detection
    void detectWheels();
    std::vector<cv::Rect> vehicles;

    // Frames
    cv::Mat mFramePreprocessed;
    cv::Mat mFrameDebug;
};

#endif // VEHICLEDETECTION_H
