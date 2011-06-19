//
// Configuration
//

// Includes
#include "tramdistance.h"
#include <iostream>

//Feature properties
#define TRACK_WIDTH 0.70  // the distance in meters betweens the tracks
#define DISTANCE_DEPTH 125 // distance shown in depth off screen in meters
#define TRAM_WIDTH 2.5    // widths in meter of a tram

//
// Construction and destruction
//

TramDistance::TramDistance(cv::Mat const* iFrame) : Component(iFrame)
{
    frameWidth = iFrame->cols;
    frameHeight = iFrame->rows;
}


//
// Component interface
//

void TramDistance::preprocess()
{
    mFrameDebug = (*frame()).clone();
}

void TramDistance::find_features(FrameFeatures& iFrameFeatures) throw(FeatureException)
{
    cv::Point trackHalfX;
    trackHalfX.y = frameHeight;

    // tracks were found
    if(iFrameFeatures.tracks.first.isEmpty()) {
        std::cout << "No tracks found "  << std::endl;
        trackHalfX.x = frameWidth/2;
        iFrameFeatures.trackHalfX = trackHalfX;
    }
    // no tracks so assume in half of the screen
    else {
        trackHalfX.x = (iFrameFeatures.tracks.first.last().x + iFrameFeatures.tracks.second.last().x)/2;
        iFrameFeatures.trackHalfX = trackHalfX;
    }

    // tram was found
    if(iFrameFeatures.tram.width != 0) {
        cv::Point tramHalfX;
        tramHalfX.y = iFrameFeatures.tram.y + iFrameFeatures.tram.height;
        tramHalfX.x = iFrameFeatures.tram.x + iFrameFeatures.tram.width/2;
        iFrameFeatures.tramHalfX = tramHalfX;


        //calculate the depth distance
        double yDistance;
        yDistance = (frameHeight - iFrameFeatures.tram.y) * DISTANCE_DEPTH / frameHeight;

        // calculate the total width is shown at that current line
        double widthDistance;
        widthDistance = frameWidth * TRAM_WIDTH / iFrameFeatures.tram.width;

        double xDistance;
        xDistance = (frameWidth/2 - tramHalfX.x) * widthDistance / frameWidth ;

        double totalDistance = yDistance * yDistance + xDistance * xDistance;
        std::cout << "totalDist: " << sqrt(totalDistance) << std::endl;

        iFrameFeatures.tramDistance = sqrt(totalDistance);
    }
    else{
        iFrameFeatures.tramHalfX = trackHalfX;
        iFrameFeatures.tramDistance = 0;
    }
}

cv::Mat TramDistance::frameDebug() const
{
    return mFrameDebug;
}
