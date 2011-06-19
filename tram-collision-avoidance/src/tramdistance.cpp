//
// Configuration
//

// Includes
#include "tramdistance.h"
#include <iostream>

//Feature properties
#define TRACK_WIDTH 0.70  // the distance in meters betweens the tracks
#define DISTANCE_DEPTH 250 // distance shown in depth off screen in meters
#define TRAM_WIDTH 2.5     // widths in meter of a tram

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
    //calculate the depth distance
    double yDistance;
    yDistance = iFrameFeatures.tram.y * DISTANCE_DEPTH / frameHeight;

    // calculate the total width is shown at that current line
    double widthDistance;
    widthDistance = frameWidth * TRAM_WIDTH / iFrameFeatures.tram.width;

    double xDistance;
    xDistance = (frameWidth/2 - iFrameFeatures.tram.x - iFrameFeatures.tram.width / 2) * widthDistance / frameWidth ;

    double totalDistance = yDistance * yDistance + xDistance * xDistance;
    std::cout << "totalDist: " << sqrt(totalDistance) << std::endl;
/*
    cv::Point trackHalfX;
    // tracks were found
    try {
        trackHalfX.y = iFrameFeatures.tracks.first[0].y;
        trackHalfX.x = (iFrameFeatures.tracks.first[0].x + iFrameFeatures.tracks.second[0].x)/2;
        iFrameFeatures.trackHalfX = trackHalfX;
    }
    catch (...)  {
        std::cout << "No tracks found "  << std::endl;
        trackHalfX.y = frameHeight;
        trackHalfX.x = frameWidth/2;
        iFrameFeatures.trackHalfX = trackHalfX;
    }

    // tram was found
    try {
        cv::Point tramHalfX;
        tramHalfX.y = iFrameFeatures.tram.y + iFrameFeatures.tram.height;
        tramHalfX.x = iFrameFeatures.tram.x + iFrameFeatures.tram.width/2;
        iFrameFeatures.tramHalfX = tramHalfX;
    }
    catch (...) {
        std::cout << "No tram found "  << std::endl;
        iFrameFeatures.tramHalfX = trackHalfX;
    }

    std::cout << "From: " << iFrameFeatures.trackHalfX.x << "," << iFrameFeatures.trackHalfX.y  << std::endl;
    std::cout << "To: " << iFrameFeatures.tramHalfX.x << "," << iFrameFeatures.tramHalfX.y << std::endl;*/
}

cv::Mat TramDistance::frameDebug() const
{
    return mFrameDebug;
}
