//
// Configuration
//

// Includes
#include "trackdetection.h"
#include "auxiliary.h"

// Feature properties
#define TRACK_WIDTH 15
#define TRACK_COUNT 2
#define TRACK_START_OFFSET_MIN 10
#define TRACK_START_OFFSET_MAX 50
#define TRACK_START_OFFSET_DELTA 5
#define TRACK_SPACE_MIN 100
#define TRACK_SPACE_MAX 175
#define SEGMENT_LENGTH_MIN 25
#define SEGMENT_LENGTH_DELTA 10
#define SEGMENT_ANGLE_MIN -M_PI_4
#define SEGMENT_ANGLE_MAX M_PI_4
#define SEGMENT_ANGLE_DELTA M_PI_4/64.0
#define SEGMENT_ANGLE_DELTA_MAX M_PI/6.0


//
// Construction and destruction
//

TrackDetection::TrackDetection(const cv::Mat& iFrame) : Component(iFrame)
{

}


//
// Component interface
//

void TrackDetection::preprocess()
{
    // Convert to grayscale
    cv::Mat tFrameGray(frame().size(), CV_8U);
    cvtColor(frame(), tFrameGray, CV_RGB2GRAY);

    // Sobel transform
    cv::Mat tFrameSobel(frame().size(), CV_16S);
    Sobel(tFrameGray, tFrameSobel, CV_16S, 3, 0, 9);

    // Convert to 32F
    cv::Mat tFrameSobelFloat(frame().size(), CV_8U);
    tFrameSobel.convertTo(tFrameSobelFloat, CV_32F, 1.0/256, 128);

    // Threshold
    cv::Mat tFrameThresholded = tFrameSobelFloat > 200;

    // Blank out useless region
    rectangle(tFrameThresholded, cv::Rect(0, 0, frame().size[1], frame().size[0] * 0.50), cv::Scalar::all(0), CV_FILLED);
    std::vector<cv::Point> tRectRight, tRectLeft;
    tRectRight.push_back(cv::Point(frame().size[1], frame().size[0]));
    tRectRight.push_back(cv::Point(frame().size[1]-frame().size[1]*0.25, frame().size[0]));
    tRectRight.push_back(cv::Point(frame().size[1], 0));
    fillConvexPoly(tFrameThresholded, &tRectRight[0], tRectRight.size(), cv::Scalar::all(0));
    tRectLeft.push_back(cv::Point(0, frame().size[0]));
    tRectLeft.push_back(cv::Point(0+frame().size[1]*0.25, frame().size[0]));
    tRectLeft.push_back(cv::Point(0, 0));
    fillConvexPoly(tFrameThresholded, &tRectLeft[0], tRectLeft.size(), cv::Scalar::all(0));

    // Save final frame
    mFramePreprocessed = tFrameThresholded;
    cvtColor(mFramePreprocessed, mFrameDebug, CV_GRAY2BGR);
}

void TrackDetection::find_features(FrameFeatures& iFrameFeatures) throw(FeatureException)
{
    // Detect lines
    std::vector<cv::Vec4i> tLines = find_lines();

    // Find track start candidates
    std::vector<cv::Point> tTrackCandidates;
    for (int tScanlineOffset = TRACK_START_OFFSET_MIN; tScanlineOffset < TRACK_START_OFFSET_MAX; tScanlineOffset += TRACK_START_OFFSET_DELTA)
    {
        unsigned int tScanline = mFramePreprocessed.size[0] - tScanlineOffset;
        tTrackCandidates = find_track_start(tLines, tScanline);
        if (tTrackCandidates.size() == 2)
            break;
    }
    for (size_t i = 0; i < tTrackCandidates.size(); i++)
        circle(mFrameDebug, tTrackCandidates[i], 5, cv::Scalar(0, 255, 255), -1);
    if (tTrackCandidates.size() != 2)
        throw FeatureException("could not find track start candidates (incorrect amount)");

    // Detect track segments
    int tTrackSpacing = abs(tTrackCandidates[0].x - tTrackCandidates[1].x);
    if (tTrackSpacing < TRACK_SPACE_MIN || tTrackSpacing > TRACK_SPACE_MAX)
        throw FeatureException("could not find track start candidates (incorrect spacing)");

    // Detect both tracks
    std::vector<cv::Point> tTrack1 = find_track(tTrackCandidates[0], tLines);
    if (tTrack1.size() <= 1)
        throw FeatureException("could not find first track");
    std::vector<cv::Point> tTrack2 = find_track(tTrackCandidates[1], tLines);
    if (tTrack2.size() <= 1)
        throw FeatureException("could not find second track");

    // TODO: detect left and right

    // TODO: check validity

    iFrameFeatures.track_left = tTrack1;
    iFrameFeatures.track_right = tTrack2;
}

void TrackDetection::copy_features(const FrameFeatures& from, FrameFeatures& to) const
{
    to.track_left = from.track_left;
    to.track_right = from.track_right;
}

cv::Mat TrackDetection::frameDebug() const
{
    return mFrameDebug;
}


//
// Feature detection
//

// Find lines in a frams
std::vector<cv::Vec4i> TrackDetection::find_lines()
{
    // Find the lines through Hough transform
    std::vector<cv::Vec4i> oLines;
    cv::HoughLinesP(mFramePreprocessed, // Image
                oLines,                 // Lines
                1,                      // Rho
                CV_PI/180,              // Theta
                20,                     // Threshold
                50,                     // Minimum line length
                3                       // Maximum line gap
                );

    // Draw lines
    for(size_t i = 0; i < oLines.size(); i++)
    {
        line(mFrameDebug,
             cv::Point(oLines[i][0],
                       oLines[i][1]),
             cv::Point(oLines[i][2],
                       oLines[i][3]),
             cv::Scalar(0,0,255),
             3,
             8
             );
    }

    return oLines;
}

// Find the start candidates of a track
std::vector<cv::Point> TrackDetection::find_track_start(const std::vector<cv::Vec4i>& iLines, unsigned int iScanline)
{
    // Find track start candidates
    std::vector<int> track_points, track_intersections;
    int y = iScanline;
    for (int x = mFramePreprocessed.size[1]-TRACK_WIDTH/2; x > TRACK_WIDTH/2; x--)
    {
        // Count the amount of segments intersecting with the current track start point
        int segments = 0;
        cv::Vec2i p1(x+TRACK_WIDTH/2, y);
        cv::Vec2i p2(x-TRACK_WIDTH/2, y);
        for(size_t i = 0; i < iLines.size(); i++)
        {
            cv::Vec2i p3(iLines[i][0], iLines[i][1]);
            cv::Vec2i p4(iLines[i][2], iLines[i][3]);

            if (intersect(p1, p2, p3, p4))
            {
                segments++;
            }
        }
        if (segments == 0)
            continue;

        // Check if we are updating an existing track
        bool tUpdateExisting = false;
        for (size_t i = 0; i < track_points.size(); i++)
        {
            if ((track_points[i] - x) < TRACK_WIDTH)
            {
                tUpdateExisting = true;
                if (track_intersections[i] < segments)
                {
                    track_intersections[i] = segments;
                    track_points[i] = x;
                }
                break;
            }
        }
        if (!tUpdateExisting)   // New track point!
        {
            if (track_points.size() < TRACK_COUNT)
            {
                track_points.push_back(x);
                track_intersections.push_back(segments);
            }
            else
            {
                // Look for the track point with the least intersecting segments
                int least = 0;
                for (size_t i = 1; i < track_points.size(); i++)
                {
                    if (track_intersections[i] < track_intersections[least])
                        least = i;
                }

                // Replace it
                track_intersections[least] = segments;
                track_points[least] = x;
            }
        }
    }

    // Draw and generate output array
    std::vector<cv::Point> oTrackCandidates;
    for (size_t i = 0; i < track_points.size(); i++)
    {
        if (track_intersections[i] % 2)                                          // Small hack to improve detection
            track_points[i] -= TRACK_WIDTH/(2 * (track_intersections[i] % 2));   // of the track center.
        int x = track_points[i];
        oTrackCandidates.push_back(cv::Point(x, y));
    }

    return oTrackCandidates;
}

// Find the segments of a track
std::vector<cv::Point> TrackDetection::find_track(const cv::Point& iStart,
                                                  const std::vector<cv::Vec4i>& iLines)
{
    // Create output vector
    std::vector<cv::Point> oTrack;
    oTrack.push_back(iStart);

    // Detect new segments
    bool tNewSegment = true;
    while (tNewSegment)
    {
        // Display segment
        cv::Point segment_last = oTrack.back();
        circle(mFrameDebug, segment_last, 5, cv::Scalar(0, 255, 0), -1);
        tNewSegment = false;

        // Convenience variables for segment center
        int x = segment_last.x;
        int y = segment_last.y;

        // Scan
        double max_overlap = 0;
        int optimal_length;
        double optimal_angle;
        for (int length = SEGMENT_LENGTH_MIN; ; length += SEGMENT_LENGTH_DELTA)
        {
            bool has_improved = false;

            for (double angle = SEGMENT_ANGLE_MIN; angle < SEGMENT_ANGLE_MAX; angle += SEGMENT_ANGLE_DELTA)
            {
                // Track segment coordinates (not rotated)
                cv::Rect r(cv::Point(x-TRACK_WIDTH/2, y),
                           cv::Point(x+TRACK_WIDTH/2, y-length));
                cv::Point r1(x - TRACK_WIDTH/2, y);
                cv::Point r2(x + TRACK_WIDTH/2, y);
                cv::Point r3(x + TRACK_WIDTH/2, y - length);
                cv::Point r4(x - TRACK_WIDTH/2, y - length);

                // Process all lines
                double tOverlap = 0;
                for(size_t i = 0; i < iLines.size(); i++)
                {
                    // Line coordinates (inversly rotated)
                    cv::Point p1(iLines[i][0], iLines[i][1]);
                    cv::Point p1_rot(x + cos(-angle)*(p1.x-x) - sin(-angle)*(p1.y-y),
                                     y + sin(-angle)*(p1.x-x) + cos(-angle)*(p1.y-y));
                    cv::Point p2(iLines[i][2], iLines[i][3]);
                    cv::Point p2_rot(x + cos(-angle)*(p2.x-x) - sin(-angle)*(p2.y-y),
                                     y + sin(-angle)*(p2.x-x) + cos(-angle)*(p2.y-y));

                    // Full overlap
                    if (r.contains(p1_rot) && r.contains(p2_rot))
                        tOverlap += sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));

                    // Partial overlap with one point contained
                    else if (r.contains(p1_rot) || r.contains(p2_rot))
                    {
                        // First point (the one within the rectangle)
                        cv::Point p3;
                        if (r.contains(p1_rot))
                            p3 = p1_rot;
                        else
                            p3 = p2_rot;

                        // Second point (intersecting the rectangle)
                        cv::Point p4;
                        if (intersect(p1_rot, p2_rot, r1, r2))
                            p4 = intersect_point(p1_rot, p2_rot, r1, r2);
                        else if (intersect(p1_rot, p2_rot, r2, r3))
                            p4 = intersect_point(p1_rot, p2_rot, r2, r3);
                        else if (intersect(p1_rot, p2_rot, r3, r4))
                            p4 = intersect_point(p1_rot, p2_rot, r3, r4);
                        else if (intersect(p1_rot, p2_rot, r4, r1))
                            p4 = intersect_point(p1_rot, p2_rot, r4, r1);

                        // Calculate the distance
                        tOverlap += sqrt((p3.x-p4.x)*(p3.x-p4.x) + (p3.y-p4.y)*(p3.y-p4.y));
                    }

                    // Partial overlap with no point contained
                    else
                    {
                        std::vector<cv::Point> p;

                        if (intersect(p1_rot, p2_rot, r1, r2))
                            p.push_back(intersect_point(p1_rot, p2_rot, r1, r2));
                        if (intersect(p1_rot, p2_rot, r2, r3))
                            p.push_back(intersect_point(p1_rot, p2_rot, r2, r3));
                        if (intersect(p1_rot, p2_rot, r3, r4))
                            p.push_back(intersect_point(p1_rot, p2_rot, r3, r4));
                        if (intersect(p1_rot, p2_rot, r4, r1))
                            p.push_back(intersect_point(p1_rot, p2_rot, r4, r1));

                        if (p.size() == 2)
                            tOverlap += sqrt((p[0].x-p[1].x)*(p[0].x-p[1].x) + (p[0].y-p[1].y)*(p[0].y-p[1].y));
                    }
                }

                // Have the new values increased the overlap?
                if (tOverlap > max_overlap)
                {
                    max_overlap = tOverlap;
                    optimal_angle = angle;
                    optimal_length = length;
                    has_improved = true;
                }
            }

            // Check if the optimal segment has been found
            // (i.e. no improvement in the last iteration)
            if (!has_improved)
            {
                // If a segment had been found, save it
                if (max_overlap > 0)
                {
                    // Track segment coordinates (forwardly rotated)
                    cv::Point r1_rot(x - cos(optimal_angle) * TRACK_WIDTH/2,
                                     y - sin(optimal_angle) * TRACK_WIDTH/2);
                    cv::Point r2_rot(x + cos(optimal_angle) * TRACK_WIDTH/2,
                                     y + sin(optimal_angle) * TRACK_WIDTH/2);
                    cv::Point r3_rot(x + cos(optimal_angle) * TRACK_WIDTH/2 + sin(optimal_angle) * optimal_length,
                                     y + sin(optimal_angle) * TRACK_WIDTH/2 - cos(optimal_angle) * optimal_length);
                    cv::Point r4_rot(x - cos(optimal_angle) * TRACK_WIDTH/2 + sin(optimal_angle) * optimal_length,
                                     y - sin(optimal_angle) * TRACK_WIDTH/2 - cos(optimal_angle) * optimal_length);

                    // Segment coordinates (center point of the top of the segment)
                    double x_new = x + optimal_length * sin(optimal_angle);
                    double y_new = y - optimal_length * cos(optimal_angle);

                    // Check the slope
                    double slope_current = atan2(y_new - oTrack.back().y,
                                                 x_new - oTrack.back().x);
                    double slope_prev;
                    if (oTrack.size() >= 2)
                    {
                        slope_prev = atan2(oTrack[oTrack.size()-1].y - oTrack[oTrack.size()-2].y,
                                           oTrack[oTrack.size()-1].x - oTrack[oTrack.size()-2].x);
                    }
                    if (oTrack.size() == 1 || abs(slope_current - slope_prev) <= SEGMENT_ANGLE_DELTA_MAX)
                    {
                        // Draw the segment
                        cv::line(mFrameDebug, r1_rot, r2_rot, cv::Scalar(0, 255, 0), 1);
                        cv::line(mFrameDebug, r2_rot, r3_rot, cv::Scalar(0, 255, 0), 1);
                        cv::line(mFrameDebug, r3_rot, r4_rot, cv::Scalar(0, 255, 0), 1);
                        cv::line(mFrameDebug, r4_rot, r1_rot, cv::Scalar(0, 255, 0), 1);

                        // Save the segment
                        oTrack.push_back(cv::Point(x_new, y_new));
                        tNewSegment = true;
                    }
                }

                // Stop the current iteration
                break;
            }
        }
    }

    return oTrack;
}
