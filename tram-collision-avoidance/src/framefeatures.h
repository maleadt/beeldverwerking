//
// Configuration
//

// Include guard
#ifndef FRAMEFEATURES_H
#define FRAMEFEATURES_H

// Includes
#include "opencv/cv.h"
#include <vector>

struct FrameFeatures
{
    std::vector<cv::Point> track_left, track_right;
};

#endif // FRAMEFEATURES_H
