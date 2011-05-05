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
    mFramePreprocessed = tFrame;
}

void TramDetection::copy_features(const FrameFeatures& from, FrameFeatures& to) const {
}

void TramDetection::find_features(FrameFeatures &iFrameFeatures) throw(FeatureException) {
    // Loading template to match with the mPreProcessedFrame
    cv::Mat tTemplate = cv::imread("../res/tram_back001.jpeg");
    if( !tTemplate.data )
        throw std::exception();

    cv::Mat tFrame;

    // Different methods for template matching
    int method[] = { CV_TM_SQDIFF, // Global minimum
                     CV_TM_SQDIFF_NORMED,
                     CV_TM_CCORR, // Global maximum
                     CV_TM_CCORR_NORMED,
                     CV_TM_CCOEFF, // Global maximum
                     CV_TM_CCOEFF_NORMED};

    // Template matching method pick
    int currMethod = 1;

    //cv::matchTemplate(mFramePreprocessed, tTemplate, mFrameDebug, method[currMethod]);

    cv::matchTemplate(mFramePreprocessed, tTemplate, tFrame, method[currMethod]);

    double tMinValue, tMaxValue;
    cv::Point tMinLocation, tMaxLocation;

    // globaal minimum en maximum vinden
    cv::minMaxLoc(tFrame, &tMinValue, &tMaxValue, &tMinLocation, &tMaxLocation);

    cv::Point tLocation;

    // Afhankelijk van gebruikte methode moet globaal minimum of globaal maximum gebruikt worden
    if(currMethod == 0 || currMethod == 1){
        // Gebruik globaal minimum
        tLocation = tMinLocation;
    } else {
        // Gebruik globaal maximum
        tLocation = tMaxLocation;
    }

    // Controle invoeren of de tram zich plots niet op een heel andere plaats bevind door middel van toegelaten afwijkingen

    cv::Point tOppositeLocaction;
    // Tegenovergestelde punten zoeken om rechthoek te tekenen
    tOppositeLocaction.y = tLocation.y + tTemplate.size().height;
    tOppositeLocaction.x = tLocation.x + tTemplate.size().width;

    iFrameFeatures.tram = cv::Rect(tLocation,tOppositeLocaction);

    mFrameDebug = tFrame;
}

cv::Mat TramDetection::frameDebug() const
{
    return mFrameDebug;
}
