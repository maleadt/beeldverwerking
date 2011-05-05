#include "tramdetection.h"

//
// Construction and destruction
//

TramDetection::TramDetection(const cv::Mat& iFrame) : Component(iFrame)
{

}

//
// Component interface
//

void TramDetection::preprocess()
{
    cv::Mat tFrame = frame();

    // Blank out useless region
    rectangle(tFrame, cv::Rect(0, 0, frame().size().width, frame().size().height * 0.10), cv::Scalar::all(0), CV_FILLED);

    std::vector<cv::Point> tRectRight, tRectLeft;
    tRectRight.push_back(cv::Point(frame().size().width, frame().size().height));
    tRectRight.push_back(cv::Point(frame().size().width-frame().size().width*0.25, frame().size().height));
    tRectRight.push_back(cv::Point(frame().size().width, 0));


    fillConvexPoly(tFrame, &tRectRight[0], tRectRight.size(), cv::Scalar::all(0));


    tRectLeft.push_back(cv::Point(0, frame().size().height));
    tRectLeft.push_back(cv::Point(0+frame().size().width*0.25, frame().size().height));
    tRectLeft.push_back(cv::Point(0, 0));
    fillConvexPoly(tFrame, &tRectLeft[0], tRectLeft.size(), cv::Scalar::all(0));

    // Save final frame
    mFrameDebug = tFrame;
}

void TramDetection::copy_features(const FrameFeatures& from, FrameFeatures& to) const {

}

void TramDetection::find_features(FrameFeatures &iFrameFeatures) throw(FeatureException) {

}

cv::Mat TramDetection::frameDebug() const
{
    return mFrameDebug;
}
