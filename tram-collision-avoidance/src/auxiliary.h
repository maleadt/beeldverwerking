//
// Configuration
//

// Include guard
#ifndef AUXILIARY_H
#define AUXILIARY_H

// Includes
#include "opencv/cv.h"
#include <QPair>

// Type definitions
typedef QPair<cv::Point, cv::Point> Line;


//
// Geometry
//

// Calculate the distance between the point and the segment.
double distance_point2segment(cv::Point iPoint, Line iSegment, cv::Point& oIntersect);

// Return True if the segments intersect.
bool intersect_segments(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersect);

// Calculate the distance between
// the segment (a.first.x, a.first.y)-(a.second.x, a.second.y) and
// the segment (b.first.x, b.first.y)-(b.second.x, b.second.y).
// Return the distance. Return the closest points
// on the segments through parameters
// (near_x1, near_y1) and (near_x2, near_y2).
double distance_segment2segment(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersectA, cv::Point& oIntersectB);

#endif // AUXILIARY_H
