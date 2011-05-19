//
// Configuration
//

// Include guard
#ifndef FRAMEFEATURES_H
#define FRAMEFEATURES_H

// Includes
#include "opencv/cv.h"
#include <vector>

struct FrameFeatures
{
    std::vector<cv::Point> track_left, track_right;
    std::vector<cv::Rect> pedestrians;
    std::vector<cv::Rect> vehicles;

    // TramDetection
    cv::Rect tram;
    cv::Point location;
    double minValue, maxValue;
};

#endif // FRAMEFEATURES_H
