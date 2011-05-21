//
// Configuration
//

// Includes
#include "trackdetection.h"
#include "auxiliary.h"
#include <limits>
#include <QDebug>

// Feature properties
#define GROUP_SLOPE_DELTA M_PI_4/4.0    // about 10 degrees
#define GROUP_DISTANCE_DELTA 30         // in pixels
#define GROUP_SIZE 2


//
// Construction and destruction
//

TrackDetection::TrackDetection(cv::Mat const* iFrame) : Component(iFrame)
{

}


//
// Component interface
//

void TrackDetection::preprocess()
{
    // Convert to grayscale
    cv::Mat tFrameGray(frame()->size(), CV_8U);
    cvtColor(*frame(), tFrameGray, CV_RGB2GRAY);

    // Sobel transform
    cv::Mat tFrameSobel(frame()->size(), CV_16S);
    Sobel(tFrameGray, tFrameSobel, CV_16S, 3, 0, 9);

    // Convert to 32F
    cv::Mat tFrameSobelFloat(frame()->size(), CV_8U);
    tFrameSobel.convertTo(tFrameSobelFloat, CV_32F, 1.0/256, 128);

    // Threshold
    cv::Mat tFrameThresholded = tFrameSobelFloat > 200;

    // Blank out useless region
    rectangle(tFrameThresholded, cv::Rect(0, 0, frame()->size().width, frame()->size().height * 0.50), cv::Scalar::all(0), CV_FILLED);
    QVector<cv::Point> tRectRight, tRectLeft;
    tRectRight.push_back(cv::Point(frame()->size().width, frame()->size().height));
    tRectRight.push_back(cv::Point(frame()->size().width-frame()->size().width*0.25, frame()->size().height));
    tRectRight.push_back(cv::Point(frame()->size().width, 0));
    fillConvexPoly(tFrameThresholded, &tRectRight[0], tRectRight.size(), cv::Scalar::all(0));
    tRectLeft.push_back(cv::Point(0, frame()->size().height));
    tRectLeft.push_back(cv::Point(0+frame()->size().width*0.25, frame()->size().height));
    tRectLeft.push_back(cv::Point(0, 0));
    fillConvexPoly(tFrameThresholded, &tRectLeft[0], tRectLeft.size(), cv::Scalar::all(0));

    // Save final frame
    mFramePreprocessed = tFrameThresholded;
    cvtColor(mFramePreprocessed, mFrameDebug, CV_GRAY2BGR);
}

void TrackDetection::find_features(FrameFeatures& iFrameFeatures) throw(FeatureException)
{
    // Detect lines
    QList<Line> tLines = find_lines();

    // Classify the lines
    QList<QList<Line> > tGroups = find_groups(tLines);
    foreach (const QList<Line>& tGroup, tGroups)
    {
        int tRandom = mRng;
        cv::Scalar tColour = CV_RGB(tRandom&255, (tRandom>>8)&255, (tRandom>>16)&255);
        foreach (const Line& tLine, tGroup)
        {
            line(mFrameDebug,
                 tLine.first,
                 tLine.second,
                 tColour,
                 3,
                 8
                 );
        }
    }

    // Generate representatives
    QList<Line> tRepresentatives = find_representatives(tGroups);
    foreach (const Line& tRepresentative, tRepresentatives)
    {
        line(mFrameDebug,
             tRepresentative.first,
             tRepresentative.second,
             cv::Scalar(0, 0, 255),
             5,
             8
             );
    }
}

cv::Mat TrackDetection::frameDebug() const
{
    return mFrameDebug;
}


//
// Feature detection
//

// Find lines in a frams
QList<Line> TrackDetection::find_lines()
{
    // Find the lines through Hough transform
    std::vector<cv::Vec4i> tLines;
    cv::HoughLinesP(mFramePreprocessed, // Image
                tLines,                 // Lines
                1,                      // Rho
                CV_PI/180,              // Theta
                20,                     // Threshold
                50,                     // Minimum line length
                3                       // Maximum line gap
                );

    // Convert to a list of lines
    QList<Line> oLines;
    for(size_t i = 0; i < tLines.size(); i++)
    {
        cv::Point tPointA(tLines[i][0], tLines[i][1]);
        cv::Point tPointB(tLines[i][2], tLines[i][3]);
        oLines.push_back(Line(tPointA, tPointB));
    }

    return oLines;
}

QList<QList<Line> > TrackDetection::find_groups(const QList<Line>& iLines)
{
    // Populate initial group
    QList<QList<Line> > oGroups;
    for (int i = 0; i < iLines.size(); i++)
    {
        QList<Line> tGroup;
        tGroup.push_back(iLines[i]);
        oGroups.push_back(tGroup);
    }

    // Classify
    bool tFlux = true;
    while (tFlux)
    {
        tFlux = false;
        QList<QList<Line> > tGroupsNew;

        // Process each old group
        while (oGroups.size() > 0)
        {
            QList<Line> tGroup = oGroups.back();
            oGroups.pop_back();

            // Check if it fits in another existing group
            if (tGroupsNew.size() > 0)
            {
                bool tFits = false;
                for (int i = 0; i < tGroupsNew.size(); i++)
                {
                    if (groups_match(tGroup, tGroupsNew[i]))
                    {
                        foreach (Line tLine, tGroup)
                            tGroupsNew[i].push_back(tLine);
                        tFits = true;
                        break;
                    }
                }

                // Doesn't fit, just add it
                if (! tFits)
                    tGroupsNew.push_back(tGroup);
            }

            // The first group cannot be matched, just add it
            else
                tGroupsNew.push_back(tGroup);
        }

        oGroups = tGroupsNew;
    }

    return oGroups;
}

QList<Line> TrackDetection::find_representatives(const QList<QList<Line> >& iGroups)
{
    QList<Line> oRepresentatives;
    foreach (const QList<Line>& tGroup, iGroups)
    {
        if (tGroup.size() >= GROUP_SIZE)
        {
            int tVerticalLowest = std::numeric_limits<int>::max(), tVerticalHighest = 0;
            double tSlopeTotal = 0;
            long tHorizontalTotal = 0;
            foreach (const Line& tLine, tGroup)
            {
                int tVerticalLow = min(tLine.first.y, tLine.second.y);
                int tVerticalHigh = max(tLine.first.y, tLine.second.y);
                if (tVerticalLow < tVerticalLowest)
                    tVerticalLowest = tVerticalLow;
                if (tVerticalHigh > tVerticalHighest)
                    tVerticalHighest = tVerticalHigh;

                int tHorizontalMidpoint = (tLine.first.x + tLine.second.x) / 2;
                tHorizontalTotal += tHorizontalMidpoint;

                double tSlope = atan2(tLine.first.y - tLine.second.y, tLine.first.x - tLine.second.x);
                tSlopeTotal += tSlope;
            }

            double tSlopeAverage = tSlopeTotal / tGroup.size();
            int tHorizontalAverage = tHorizontalTotal / tGroup.size();
            double tHorizontalLength = (tVerticalHighest - tVerticalLowest) / tan(tSlopeAverage);

            cv::Point tBottom(tHorizontalAverage - tHorizontalLength/2, tVerticalLowest);
            cv::Point tTop(tHorizontalAverage + tHorizontalLength/2, tVerticalHighest);
            oRepresentatives.append(Line(tBottom, tTop));
        }
    }

    return oRepresentatives;
}


//
// Auxiliary math
//

// Check if two groups match
bool TrackDetection::groups_match(const QList<Line>& iGroupA, const QList<Line>& iGroupB)
{
    foreach (const Line& tLineA, iGroupA)
    {
        double tSlopeA = abs(atan2(tLineA.first.y - tLineA.second.y, tLineA.first.x - tLineA.second.x));

        foreach (const Line& tLineB, iGroupB)
        {
            double tSlopeB = abs(atan2(tLineB.first.y - tLineB.second.y, tLineB.first.x - tLineB.second.x));

            // Almost parallel
            if (abs(tSlopeA - tSlopeB) <= GROUP_SLOPE_DELTA)
            {
                cv::Point tPointA, tPointB;
                double tDistance = distance_segment2segment(tLineA, tLineB, tPointA, tPointB);
                if (tDistance < GROUP_DISTANCE_DELTA)
                    return true;
            }
        }
    }

    return false;
}

// Calculate the distance between
// the segment (a.first.x, a.first.y)-(a.second.x, a.second.y) and
// the segment (b.first.x, b.first.y)-(b.second.x, b.second.y).
// Return the distance. Return the closest points
// on the segments through parameters
// (near_x1, near_y1) and (near_x2, near_y2).
double TrackDetection::distance_segment2segment(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersectA, cv::Point& oIntersectB) const
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

// Calculate the distance between the point and the segment.
double TrackDetection::distance_point2segment(cv::Point iPoint, Line iSegment, cv::Point& oIntersect) const
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
bool TrackDetection::intersect_segments(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersect) const
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
