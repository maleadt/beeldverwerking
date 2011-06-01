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
    // Initializing ROI
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


    // Save final frame
    mFrameDebug = frame()->clone();
    mFramePreprocessed = frame()->clone();
}

void TramDetection::find_features(FrameFeatures &iFrameFeatures) throw(FeatureException) {
    // Cropping
    cv::Mat cropFrame = frame()->clone();

    cv::Rect tROI(mROIPoint,mROISize);

    mFrameDebug = cropFrame(tROI);
    mFramePreprocessed = cropFrame(tROI);

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
    cv::Point tLocationCropped;

    // Dependent on the method used you have to use the local minimum or maximum
    if(currMethod < 3 ) {
        // Is there a tram

        //        std::cout << "MinValue=" << tMinValue;

        if(tMinValue > MIN_THRESHOLD){
            throw FeatureException("no tram found (" + QString::number(tMaxValue) + ")");
        }
        // Use global minimum
        tLocationCropped = tMinLocation;
        iFrameFeatures.minValue = tMinValue;
    } else {
        // Is there a tram

        //        std::cout << "MaxValue=" << tMaxValue;

        if(tMaxValue < MAX_THRESHOLD){
            throw FeatureException("no tram found (" + QString::number(tMaxValue) + ")");
        }
        // Use global maximum
        tLocationCropped = tMaxLocation;
        iFrameFeatures.maxValue = tMaxValue;
    }

    cv::Point tOppositeLocactionCropped;
    // Finding opposite corner to draw the rectangle for cropped mFrameDebug
    tOppositeLocactionCropped.y = tLocationCropped.y + tTemplate.size().height;
    tOppositeLocactionCropped.x = tLocationCropped.x + tTemplate.size().width;

    cv::rectangle(mFrameDebug, tLocationCropped, tOppositeLocactionCropped, cv::Scalar(0, 255, 0), 1);

    // Adjusting the point to fit on the original frame
    cv::Point tLocation = cv::Point(tLocationCropped.x + mROIPoint.x, tLocationCropped.y + mROIPoint.y);

    cv::Point tOppositeLocaction;
    // Finding opposite corner to draw the rectangle
    tOppositeLocaction.y = tLocation.y + tTemplate.size().height;
    tOppositeLocaction.x = tLocation.x + tTemplate.size().width;

//    // Check wether the abnormalities are valid
//    if(tLocation.x - iFrameFeatures.location.x > DELTA_X){

//    }





    // Save the features
    if(currMethod < 3 ) {
        iFrameFeatures.minValue = tMinValue;
    } else {
        iFrameFeatures.maxValue = tMaxValue;
    }
    iFrameFeatures.location = tLocation;
    iFrameFeatures.tram = cv::Rect(tLocation, tTemplate.size());
}

void TramDetection::calculate_croparea(FrameFeatures &iFrameFeatures){
//    // Calculating rectangle to crop to crop
//    int leftMinX = 0, leftMaxX = 0, rightMinX = 0, rightMaxX = 0, leftMinY = 0, leftMaxY = 0, rightMinY = 0, rightMaxY = 0;
//    cv::Point leftUpperLeft, leftLowerRight, rightUpperRight, rightLowerLeft;

//    if(!iFrameFeatures.track_left.empty()){

//        for (size_t i = 0; i < iFrameFeatures.track_left.size()-1; i++) {
//            if(iFrameFeatures.track_left[i].x < leftMinX){
//                leftMinX = iFrameFeatures.track_left[i].x;
//            }
//            else if(iFrameFeatures.track_left[i].x > leftMaxX){
//                leftMaxX = iFrameFeatures.track_left[i].x;
//            }
//            else if(iFrameFeatures.track_left[i].y < leftMinY){
//                leftMinY = iFrameFeatures.track_left[i].y;
//            }
//            else if(iFrameFeatures.track_left[i].x > leftMaxY){
//                leftMaxY = iFrameFeatures.track_left[i].y;
//            }
//        }

//        //        iFrameFeatures.leftUpperLeft = cv::Point(leftMinX,leftMaxY);
//        //        iFrameFeatures.leftLowerRight = cv::Point(leftMaxX,leftMinY);
//        mLeftUpperLeft = cv::Point(leftMinX,leftMaxY);
//        mLeftLowerRight = cv::Point(leftMaxX,leftMinY);
//    }
//    //    cv::line(mFrameDebug, iFrameFeatures.leftUpperLeft, iFrameFeatures.leftLowerRight, cv::Scalar(0, 0, 255));
//    cv::line(mFramePreprocessed, mLeftUpperLeft, mLeftLowerRight, cv::Scalar(0, 0, 255));


//    if(!iFrameFeatures.track_right.empty()){
//        for (size_t i = 0; i < iFrameFeatures.track_right.size()-1; i++){
//            if(iFrameFeatures.track_right[i].x < rightMinX){
//                rightMinX = iFrameFeatures.track_right[i].x;
//            }
//            else if(iFrameFeatures.track_right[i].x > rightMaxX){
//                rightMaxX = iFrameFeatures.track_right[i].x;
//            }
//            else if(iFrameFeatures.track_right[i].y < rightMinY){
//                rightMinY = iFrameFeatures.track_right[i].y;
//            }
//            else if(iFrameFeatures.track_right[i].x > rightMaxY){
//                rightMaxY = iFrameFeatures.track_right[i].y;
//            }
//        }

//        //        iFrameFeatures.rightUpperRight = cv::Point(rightMaxX,rightMaxY);
//        //        iFrameFeatures.rightLowerLeft = cv::Point(leftMinX,leftMinY);

//        mRightUpperRight = cv::Point(rightMaxX,rightMaxY);
//        mRightLowerLeft = cv::Point(leftMinX,leftMinY);
//    }
//    //    cv::line(mFrameDebug, iFrameFeatures.rightLowerLeft,iFrameFeatures.rightUpperRight, cv::Scalar(0, 0, 255));
//    cv::line(mFramePreprocessed, mRightLowerLeft,mRightUpperRight, cv::Scalar(0, 0, 255));
}

cv::Mat TramDetection::frameDebug() const
{
    return mFrameDebug;
}
