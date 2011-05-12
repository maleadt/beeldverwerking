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
    Component(cv::Mat const* iFrame) : mFrame(iFrame)
    {
    }

    /*
      The preprocess() method preprocesses the current frame and. The stuff
      you do during this step needs to be independant from other frame
      features (like, some thresholding, blurring, etcetera).
      */
    virtual void preprocess() = 0;

    /*
      This method actually detects the features in the (likely preprocessed)
      frame, and saves them in the passed FrameFeatures struct ref. The method
      may depend on previously detected features. Throw an exception as soon as
      something fails, don't just stop looking for features, as the main
      application depends on this.
      */
    virtual void find_features(FrameFeatures&) throw(FeatureException) = 0;

    /*
      The current frame to be processed.
      */

    cv::Mat const* frame() const
    {
        return mFrame;
    }


    virtual cv::Mat frameDebug() const = 0;

private:
    cv::Mat const* mFrame;
};

#endif // COMPONENT_H
