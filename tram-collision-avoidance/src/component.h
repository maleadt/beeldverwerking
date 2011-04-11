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
    /*
      A component object is spawned for each and every frame, and the
      current frame is passed using this constructor. No need to save
      or copy the object in an implementing class, as the Component
      superclass already does this and saves the result in a protected
      variable mFrame.
      */
    Component(const cv::Mat& iFrame) : mFrame(iFrame)
    {
    }

    /*
      The preprocess() method preprocesses the current frame mFrame and
      saves the result into mFramePreprocessed. The stuff you do during this
      step needs to be independant from other frame features (like, some
      thresholding, blurring, etcetera).
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
      This method copies features from a previous result to a new variable. It
      is called by the application when the find_features() call threw an exception,
      and the application decides to re-use a previous result.
      */
    virtual void copy_features(const FrameFeatures& from, FrameFeatures& to) const = 0;

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
    /*
      The current frame to be processed.
      */
    const cv::Mat& mFrame;

    /*
      A frame to write some debugging information to. It is only
      initialized after the preprocess() call (as it will contain
      a copy of the preprocessed frame), so only use it to
      debug the find_features() call.
      */
    cv::Mat mFrameDebug;

    /*
      The preprocessed frame. Fill this variable during the
      preprocess() method.
      */
    cv::Mat mFramePreprocessed;
};

#endif // COMPONENT_H
