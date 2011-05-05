#ifndef PEDESTRIANDETECTION_H
#define PEDESTRIANDETECTION_H
// Includes
//#include "opencv/cv.h"
//#include "opencv2/core/core.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <vector>
#include "component.h"
#include "framefeatures.h"

class PedestrianDetection : public Component
{
public:
    // Construction and destruction
    PedestrianDetection(const cv::Mat& iFrame);

    // Component interface
    void preprocess();
    void find_features(FrameFeatures& iFrameFeatures) throw(FeatureException);
    void copy_features(const FrameFeatures& from, FrameFeatures& to) const;
    cv::Mat frameDebug() const;

private:
    // Feature detection
    cv::CascadeClassifier cascade;
    int scale;
    int railsStartCol, railsEndCol;

    void cropFrame();
    void enhanceFrame();
    void detectPedestrians(FrameFeatures& iFrameFeatures);

    cv::Mat mFrameCropped;

    // Frames
    cv::Mat mFramePreprocessed;
    cv::Mat mFrameDebug;
};
#endif // PEDESTRIANDETECTION_H
