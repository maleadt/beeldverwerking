//
// Configuration
//

// Include guard
#ifndef COMPONENT_H
#define COMPONENT_H

// Includes
#include "opencv/cv.h"
#include "framefeatures.h"

class Component
{
public:
    Component(const cv::Mat& iFrame) : mFrame(iFrame)
    {
    }

    virtual void preprocess() = 0;
    virtual void find_features(FrameFeatures&) = 0;

    cv::Mat frameDebug() const
    {
        return mFrameDebug;
    }
    void setFrameDebug(const cv::Mat& iFrameDebug)
    {
        // TODO: if not set, don't draw
        mFrameDebug = iFrameDebug.clone();
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
