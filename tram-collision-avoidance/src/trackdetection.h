//
// Configuration
//

// Include guard
#ifndef TRACKDETECTION_H
#define TRACKDETECTION_H

// Includes
#include "opencv/cv.h"
#include <cmath>
#include <vector>
#include "component.h"

class TrackDetection : public Component
{
public:
    // Construction and destruction
    TrackDetection(const cv::Mat& iFrame);

    // Component interface
    void preprocess();
    void find_features(FrameFeatures& iFrameFeatures);

private:
    // Feature detection
    std::vector<cv::Vec4i> find_lines();
    std::vector<cv::Point> find_track_start(const std::vector<cv::Vec4i>& iLines);
    std::vector<cv::Point> find_track(const cv::Point& iStart, const std::vector<cv::Vec4i>& iLines);
};

#endif // TRACKDETECTION_H
