#ifndef TRAMDISTANCE_H
#define TRAMDISTANCE_H

// Includes

#include "opencv/cv.h"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "component.h"
#include "framefeatures.h"

class TramDistance : public Component
{
public:
    // Construction and destruction
    TramDistance(cv::Mat const* iFrame);

    // Component interface
    void preprocess();
    void find_features(FrameFeatures& iFrameFeatures) throw(FeatureException);
    cv::Mat frameDebug() const;

private:
    cv::Mat mFrameCropped;

    int frameHeight;
    int frameWidth;

    // Frames
    cv::Mat mFramePreprocessed;
    cv::Mat mFrameDebug;
};

#endif // TRAMDISTANCE_H
