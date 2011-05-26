//
// Configuration
//

// Includes
#include "auxiliary.h"


//
// Geometry
//

// Calculate the distance between the point and the segment.
double distance_point2segment(cv::Point iPoint, Line iSegment, cv::Point& oIntersect)
{
    double tDx = iSegment.second.x - iSegment.first.x;
    double tDy = iSegment.second.y - iSegment.first.y;
    if (tDx == 0 && tDy == 0)
    {
        // It's a point not a line segment.
        tDx = iPoint.x - iSegment.first.x;
        tDy = iPoint.y - iSegment.first.y;
        oIntersect = iSegment.first;
        return sqrt(tDx * tDx + tDy * tDy);
    }

    // Calculate the t that minimizes the distance.
    double t = ((iPoint.x - iSegment.first.x) * tDx + (iPoint.y - iSegment.first.y) * tDy) / (tDx * tDx + tDy * tDy);

    // See if this represents one of the segment's
    // end points or a point in the middle.
    if (t < 0)
    {
        tDx = iPoint.x - iSegment.first.x;
        tDy = iPoint.y - iSegment.first.y;
        oIntersect = iSegment.first;
    }
    else if (t > 1)
    {
        tDx = iPoint.x - iSegment.second.x;
        tDy = iPoint.y - iSegment.second.y;
        oIntersect = iSegment.second;
    }
    else
    {
        oIntersect.x = iSegment.first.x + t * tDx;
        oIntersect.y = iSegment.first.y + t * tDy;
        tDx = iPoint.x - oIntersect.x;
        tDy = iPoint.y - oIntersect.y;
    }

    return sqrt(tDx * tDx + tDy * tDy);
}

// Return True if the segments intersect.
bool intersect_segments(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersect)
{
    double tDxA = iSegmentA.second.x - iSegmentA.first.x;
    double tDyA = iSegmentA.second.y - iSegmentA.first.y;
    double tDxB = iSegmentB.second.x - iSegmentB.first.x;
    double tDyB = iSegmentB.second.y - iSegmentB.first.y;
    if ((tDxB * tDyA - tDyB * tDxA) == 0)
    {
        // The segments are parallel.
        return false;
    }

    double s = (tDxA * (iSegmentB.first.y - iSegmentA.first.y) + tDyA * (iSegmentA.first.x - iSegmentB.first.x)) / (tDxB * tDyA - tDyB * tDxA);
    double t = (tDxB * (iSegmentA.first.y - iSegmentB.first.y) + tDyB * (iSegmentB.first.x - iSegmentA.first.x)) / (tDyB * tDxA - tDxB * tDyA);

    oIntersect.x = iSegmentA.first.x + t * tDxA;
    oIntersect.y = iSegmentA.first.y + t * tDyA;

    return ((s >= 0 && s <= 1 && t >= 0 && t <= 1));
}

// Calculate the distance between
// the segment (a.first.x, a.first.y)-(a.second.x, a.second.y) and
// the segment (b.first.x, b.first.y)-(b.second.x, b.second.y).
// Return the distance. Return the closest points
// on the segments through parameters
// (near_x1, near_y1) and (near_x2, near_y2).
double distance_segment2segment(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersectA, cv::Point& oIntersectB)
{
    // See if the segments intersect.
    cv::Point tIntersect;
    if (intersect_segments(iSegmentA, iSegmentB, tIntersect))
    {
        return 0;
        oIntersectA = tIntersect;
        oIntersectA = tIntersect;
    }

    // Check (a.first.x, a.first.y) with segment 2.
    double tDistanceCurrent = distance_point2segment(cv::Point(iSegmentA.first.x, iSegmentA.first.y), iSegmentB, tIntersect);
    double tDistanceBest = std::numeric_limits<double>::max();
    if (tDistanceCurrent < tDistanceBest)
    {
        tDistanceBest = tDistanceCurrent;
        oIntersectA = tIntersect;
        oIntersectB = iSegmentA.first;
    }

    // Check (a.second.x, a.second.y) with segment 2.
    tDistanceCurrent = distance_point2segment(cv::Point(iSegmentA.second.x, iSegmentA.second.y), iSegmentB, tIntersect);
    if (tDistanceCurrent < tDistanceBest)
    {
        tDistanceBest = tDistanceCurrent;
        oIntersectA = iSegmentA.second;
        oIntersectB = tIntersect;
    }

    // Check (b.first.x, b.first.y) with segment 1.
    tDistanceCurrent = distance_point2segment(cv::Point(iSegmentB.first.x, iSegmentB.first.y), iSegmentA, tIntersect);
    if (tDistanceCurrent < tDistanceBest)
    {
        tDistanceBest = tDistanceCurrent;
        oIntersectA = tIntersect;
        oIntersectB = iSegmentB.first;
    }

    // Check (b.second.x, b.second.y) with segment 1.
    tDistanceCurrent = distance_point2segment(cv::Point(iSegmentB.second.x, iSegmentB.second.y), iSegmentA, tIntersect);
    if (tDistanceCurrent < tDistanceBest)
    {
        tDistanceBest = tDistanceCurrent;
        oIntersectA = tIntersect;
        oIntersectB = iSegmentB.second;
    }

    return tDistanceBest;
}
