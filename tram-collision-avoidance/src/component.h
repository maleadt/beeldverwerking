//
// Configuration
//

// Include guard
#ifndef COMPONENT_H
#define COMPONENT_H

// Includes
#include "opencv/cv.h"
#include "framefeatures.h"
#include "featureexception.h"

class Component
{
public:
    Component(const cv::Mat& iFrame) : mFrame(iFrame)
    {
    }

    virtual void preprocess() = 0;
    virtual void find_features(FrameFeatures&) throw(FeatureException) = 0;

    cv::Mat frameDebug() const
    {
        return mFrameDebug;
    }
    void setFrameDebug(const cv::Mat& iFrameDebug)
    {
        // TODO: if not set, don't draw
        mFrameDebug = iFrameDebug.clone();
        if (mFrameDebug.depth() == 1)
            cvtColor(mFrameDebug, mFrameDebug, CV_GRAY2BGR);
    }

    cv::Mat framePreprocessed() const
    {
        return mFramePreprocessed;
    }

protected:
    const cv::Mat& mFrame;
    cv::Mat mFrameDebug;
    cv::Mat mFramePreprocessed;
};

#endif // COMPONENT_H
