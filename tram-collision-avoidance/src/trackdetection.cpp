//
// Configuration
//

// Includes
#include "trackdetection.h"
#include <limits>
#include <QDebug>

// Feature properties
#define GROUP_SLOPE_DELTA M_PI_4/8.0    // about 5 degrees
#define GROUP_DISTANCE_DELTA 15         // in pixels
#define GROUP_SIZE 2
#define STITCH_SLOPE_DELTA M_PI_4       // 45 degrees
#define STITCH_DISTANCE_DELTA_X 30
#define STITCH_DISTANCE_DELTA_Y 60
#define TRACK_SPACE_MIN 100
#define TRACK_SPACE_MAX 175


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
            cv::line(mFrameDebug,
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
        cv::line(mFrameDebug,
             tRepresentative.first,
             tRepresentative.second,
             cv::Scalar(0, 0, 255),
             5,
             8
             );
    }

    // Stitch representative lines
    QList<Track > tStitches = find_stitches(tRepresentatives);
    foreach (const Track& tStitch, tStitches)
    {
        Track::const_iterator tIterator = tStitch.begin();
        cv::Point tPrevious = *tIterator++;
        while (tIterator != tStitch.end())
        {
            cv::Point tPoint = *tIterator++;
            cv::line(mFrameDebug,
                 tPrevious,
                 tPoint,
                 cv::Scalar(0, 255, 0),
                 5,
                 8
                 );
            tPrevious = tPoint;
        }
    }

    // Find valid tracks
    TrackStart tTrackStart;
    QPair<Track, Track> tTramTrack;
    if (find_trackstart(tStitches, 10, tTrackStart, tTramTrack))
    {
        cv::circle(mFrameDebug,
                   tTrackStart.first,
                   8,
                   cv::Scalar(0, 255, 255),
                   2);
        cv::circle(mFrameDebug,
                   tTrackStart.second,
                   8,
                   cv::Scalar(0, 255, 255),
                   2);
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
                15,                     // Threshold
                40,                     // Minimum line length
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
                else
                    tFlux = true;
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
                cv::Point tLineLow = tLine.first, tLineHigh = tLine.second;
                if (tLineLow.y > tLineHigh.y)
                    swap(tLineLow, tLineHigh);

                if (tLineLow.y < tVerticalLowest)
                    tVerticalLowest = tLineLow.y;
                if (tLineHigh.y > tVerticalHighest)
                    tVerticalHighest = tLineHigh.y;

                int tHorizontalMidpoint = (tLine.first.x + tLine.second.x) / 2;
                tHorizontalTotal += tHorizontalMidpoint;

                double tSlope = atan2(tLineLow.y - tLineHigh.y, tLineLow.x - tLineHigh.x);
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

QList<Track > TrackDetection::find_stitches(const QList<Line>& iRepresentatives)
{
    QList<Track > oStitches;
    foreach (Line tRepresentative, iRepresentatives)
    {
        Track tInitialStitch;
        tInitialStitch.append(tRepresentative.first);
        tInitialStitch.append(tRepresentative.second);
        oStitches.append(tInitialStitch);
    }

    bool tFlux = true;
    while (tFlux)
    {
        tFlux = false;
        QList<Track > tStitchesNew;

        while (oStitches.size() > 0)
        {
            Track tStitch = oStitches.back();
            oStitches.pop_back();

            // Check if it fits in another existing group of stitches
            if (tStitchesNew.size() > 0)
            {
                bool tFits = false;
                for (int i = 0; i < tStitchesNew.size(); i++)
                {
                    cv::Point tStitchPoint;
                    if (stitches_match(tStitchesNew[i], tStitch, tStitchPoint))
                    {
                        tStitchesNew[i].pop_back();
                        tStitchesNew[i].append(tStitchPoint);
                        tStitch.pop_front();
                        foreach (cv::Point tPoint, tStitch)
                            tStitchesNew[i].push_back(tPoint);
                        tFits = true;
                        break;
                    }
                }

                // Doesn't fit, just add it
                if (! tFits)
                    tStitchesNew.push_back(tStitch);
                else
                    tFlux = true;
            }

            // The first stitch cannot be matched, just add it
            else
                tStitchesNew.push_back(tStitch);
        }

        oStitches = tStitchesNew;
    }

    return oStitches;
}


bool TrackDetection::find_trackstart(const QList<Track >& iStitches, int iScanlineOffset, TrackStart& oTrackStart, QPair<Track, Track>& oTracks)
{
    int tScanlineHeight = mFramePreprocessed.size().height - iScanlineOffset;
    Line tScanline(
                cv::Point(0, tScanlineHeight),
                cv::Point(mFramePreprocessed.size().width, tScanlineHeight)
                );

    Track tScanlineIntersections;
    QMap<int, Track> tTrackMap;
    foreach (Track tStitch, iStitches)
    {
        Track::const_iterator tIterator = tStitch.end();
        Line tLowestSegment;
        tLowestSegment.second = *--tIterator;
        tLowestSegment.first = *--tIterator;

        cv::Point tTrackStart;
        if (intersect_segments(tScanline, tLowestSegment, tTrackStart))
        {
            tScanlineIntersections.append(tTrackStart);
            tTrackMap.insert(tTrackStart.x, tStitch);
        }
    }

    QList<TrackStart> tTrackStarts;
    while (tScanlineIntersections.size() > 0)
    {
        cv::Point tPointA = tScanlineIntersections.back();
        tScanlineIntersections.pop_back();

        Track::iterator tIterator = tScanlineIntersections.begin();
        while (tIterator != tScanlineIntersections.end())
        {
            cv::Point tPointB = *tIterator;
            double tDistance = abs(tPointA.x - tPointB.x);
            if (tDistance > TRACK_SPACE_MIN && tDistance < TRACK_SPACE_MAX)
            {
                tTrackStarts.append(TrackStart(tPointA, tPointB));
                tScanlineIntersections.erase(tIterator);
                break;
            }

            tIterator++;
        }
    }
    if (tTrackStarts.size() > 0)
    {
        int tBestTrackStartPosition = std::numeric_limits<int>::max();
        foreach (TrackStart tTrackStart, tTrackStarts)
        {
            int tTrackStartPosition = (tTrackStart.first.x + tTrackStart.second.x)/2 - mFrameDebug.size().width/2;
            if (abs(tTrackStartPosition) < abs(tBestTrackStartPosition))
            {
                oTrackStart = tTrackStart;
                oTracks.first = tTrackMap.value(tTrackStart.first.x);
                oTracks.second = tTrackMap.value(tTrackStart.second.x);
            }
        }
        return true;
    }
    return false;
}


//
// Auxiliary
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

// Check if two stitches match
bool TrackDetection::stitches_match(const Track& iStitchA, const Track& iStitchB, cv::Point& oIntersection)
{
    // Extract the relevant segments
    Track::const_iterator tIterator = iStitchA.end();
    Line tLineA;
    tLineA.second = *--tIterator;
    tLineA.first = *--tIterator;
    tIterator = iStitchB.begin();
    Line tLineB;
    tLineB.first = *tIterator++;
    tLineB.second = *tIterator++;

    double tSlopeA = abs(atan2(tLineA.first.y - tLineA.second.y, tLineA.first.x - tLineA.second.x));
    double tSlopeB = abs(atan2(tLineB.first.y - tLineB.second.y, tLineB.first.x - tLineB.second.x));

    if (abs(tSlopeA - tSlopeB) <= STITCH_SLOPE_DELTA)
    {
        int tDistanceX = abs(tLineA.second.x - tLineB.first.x);
        int tDistanceY = abs(tLineA.second.y - tLineB.first.y);
        if (tDistanceX <= STITCH_DISTANCE_DELTA_X && tDistanceY <= STITCH_DISTANCE_DELTA_Y)
        {
            oIntersection.x = (tLineA.second.x + tLineB.first.x)/2;
            oIntersection.y = (tLineA.second.y + tLineB.first.y)/2;
            return true;
        }
    }

    return false;
}
