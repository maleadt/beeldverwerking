//
// Configuration
//

// Includes
#include "tramdetection.h"
#include <QString>
#include <iostream>

// Feature properties
#define MAX_THRESHOLD 0.895
#define MIN_THRESHOLD 0
#define DELTA_X 100
#define DELTA_Y 100

//
// Construction and destruction
//

TramDetection::TramDetection(cv::Mat const* iFrame) : Component(iFrame)
{
    mROIPoint = cv::Point(iFrame->size().width*0.33,0);
    mROISize = cv::Size(iFrame->size().width*0.33,iFrame->size().height);
}

//
// Component interface
//

void TramDetection::preprocess()
{
    //    // Blank out useless region
    //    rectangle(tFrame, cv::Rect(0, 0, frame()->size().width, frame()->size().height * 0.10), cv::Scalar::all(0), CV_FILLED);

    //    std::vector<cv::Point> tRectRight, tRectLeft;
    //    tRectRight.push_back(cv::Point(frame()->size().width, frame()->size().height));
    //    tRectRight.push_back(cv::Point(frame()->size().width-frame()->size().width*0.25, frame()->size().height));
    //    tRectRight.push_back(cv::Point(frame()->size().width, 0));

    //    fillConvexPoly(tFrame, &tRectRight[0], tRectRight.size(), cv::Scalar::all(0));
    //    tRectLeft.push_back(cv::Point(0, frame()->size().height));
    //    tRectLeft.push_back(cv::Point(0+frame()->size().width*0.25, frame()->size().height));
    //    tRectLeft.push_back(cv::Point(0, 0));
    //    fillConvexPoly(tFrame, &tRectLeft[0], tRectLeft.size(), cv::Scalar::all(0));

    // Copy the input frame
    cv::Mat tFrame = frame()->clone();

    cv::Rect tROI(mROIPoint,mROISize);

    tFrame = tFrame(tROI);

    // Save final frame
    mFrameDebug = tFrame;
    mFramePreprocessed = tFrame;
}

void TramDetection::find_features(FrameFeatures &iFrameFeatures) throw(FeatureException) {
    // Adjust cropping to tracks even more?

    // Loading template to match with the mPreProcessedFrame
    cv::Mat tTemplate = cv::imread("../res/tram_back004.jpg");
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
    int currMethod = 3;

    cv::matchTemplate(mFramePreprocessed, tTemplate, tFrame, method[currMethod]);

    double tMinValue, tMaxValue;
    cv::Point tMinLocation, tMaxLocation;

    // Finding global minimum and maximum
    cv::minMaxLoc(tFrame, &tMinValue, &tMaxValue, &tMinLocation, &tMaxLocation);

    // Adjusting the point to fit on the original frame
    cv::Point tLocation;
    tLocation = cv::Point(tLocation.x, tLocation.y);

    // Afhankelijk van gebruikte methode moet globaal minimum of globaal maximum gebruikt worden
    if(currMethod < 3 ) {
        // Is er echt wel een tram?
        //        std::cout << "MinValue=" << tMinValue;
        if(tMinValue > MIN_THRESHOLD){
            throw FeatureException("no tram found (" + QString::number(tMaxValue) + ")");
        }
        // Gebruik globaal minimum
        tLocation = tMinLocation;
        iFrameFeatures.minValue = tMinValue;
    } else {
        // Is er echt wel een tram?
        //        std::cout << "MaxValue=" << tMaxValue;
        if(tMaxValue < MAX_THRESHOLD){
            throw FeatureException("no tram found (" + QString::number(tMaxValue) + ")");
        }
        // Gebruik globaal maximum
        tLocation = tMaxLocation;
        iFrameFeatures.maxValue = tMaxValue;
    }


    // Controle invoeren of de tram zich plots niet op een heel andere plaats bevind door middel van toegelaten afwijkingen
    if(tLocation.x - iFrameFeatures.location.x > DELTA_X){

    }

    cv::Point tOppositeLocaction;
    // Tegenovergestelde punten zoeken om rechthoek te tekenen
    tOppositeLocaction.y = tLocation.y + tTemplate.size().height;
    tOppositeLocaction.x = tLocation.x + tTemplate.size().width;

    // Opslaan van de gevonden waarden om bij volgende frame te gebruiken
    if(currMethod < 3 ) {
        iFrameFeatures.minValue = tMinValue;
    } else {
        iFrameFeatures.maxValue = tMaxValue;
    }
    iFrameFeatures.location = tLocation;
    iFrameFeatures.tram = cv::Rect(tLocation,tOppositeLocaction);
}

cv::Mat TramDetection::frameDebug() const
{
    return mFrameDebug;
}
