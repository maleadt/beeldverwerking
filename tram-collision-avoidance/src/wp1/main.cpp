///////////////////////////////////////////////////////////////////////////////
// Configuration
//

// Headers
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <cmath>
#include <iostream>
#include <vector>

// Feature properties
#define TRACK_WIDTH 15
#define TRACK_COUNT 2
#define TRACK_START_OFFSET 10
#define TRACK_SPACE_MIN 100
#define TRACK_SPACE_MAX 175
#define SEGMENT_LENGTH_MIN 25
#define SEGMENT_LENGTH_DELTA 10
#define SEGMENT_ANGLE_MIN -M_PI_4
#define SEGMENT_ANGLE_MAX M_PI_4
#define SEGMENT_ANGLE_DELTA M_PI_4/64.0
#define SEGMENT_ANGLE_DELTA_MAX M_PI/6.0


///////////////////////////////////////////////////////////////////////////////
// Routines
//

int twz(cv::Point a, cv::Point b, cv::Point c)
{
    int dxb = b.x - a.x, dyb = b.y - a.y,
            dxc = c.x - a.x, dyc = c.y - a.y;
    if (dxb * dyc > dyb * dxc)
        return 1;
    else if (dxb * dyc < dyb * dxc)
        return -1;
    else if (dxb * dxc < 0 || dyb * dyc < 0)
        return -1;
    else if (dxb * dxb + dyb * dyb >= dxc * dxc + dyc * dyc)
        return 0;
    else
        return 1;
}

bool intersect(const cv::Point &p1, const cv::Point &p2,
               const cv::Point &p3, const cv::Point &p4)
{
    return     (twz(p1, p2, p3) * twz(p1, p2, p4) <= 0)
            && (twz(p3, p4, p1) * twz(p3, p4, p2) <= 0);
}

// Intersection has to be tested before this method is called.
cv::Point intersect_point(const cv::Point &p1, const cv::Point &p2,
                          const cv::Point &p3, const cv::Point &p4)
{
    int d = (p1.x-p2.x)*(p3.y-p4.y) - (p1.y-p2.y)*(p3.x-p4.x);

    // HACK HACK
    if (d == 0)
        d++;

    int xi = ((p3.x-p4.x)*(p1.x*p2.y-p1.y*p2.x)
              -(p1.x-p2.x)*(p3.x*p4.y-p3.y*p4.x))
            /d;
    int yi = ((p3.y-p4.y)*(p1.x*p2.y-p1.y*p2.x)
              -(p1.y-p2.y)*(p3.x*p4.y-p3.y*p4.x))
            /d;

    return cv::Point(xi, yi);
}

///////////////////////////////////////////////////////////////////////////////
// Main
//

int main(int argc, char** argv)
{
    //
    // Setup application
    //

    // Read command-line parameters
    if (argc < 2)
    {
        std::cout << "Error: invalid usage" << "\n";
        std::cout << "Usage: ./wp1 <input_filename> <output_filename>" << std::endl;
        return 1;
    }

    // Open input video
    std::string iVideoFile = argv[1];
    cv::VideoCapture iVideo(iVideoFile);
    if(!iVideo.isOpened())
    {
        std::cout << "Error: could not open video" << std::endl;
        return 1;
    }

    // Open output video
    cv::VideoWriter oVideo;
    if (argc == 3)
    {
        std::string oVideoFile = argv[2];
        oVideo = cv::VideoWriter(oVideoFile,
                                 CV_FOURCC('M', 'J', 'P', 'G'),
                                 iVideo.get(CV_CAP_PROP_FPS),
                                 cv::Size(iVideo.get(CV_CAP_PROP_FRAME_WIDTH),iVideo.get(CV_CAP_PROP_FRAME_HEIGHT)),
                                 true);
    }

    // Setup OpenCV
    cv::namedWindow("wp1", 1);


    //
    // Process video
    //

    cv::Mat tFrame;
    while (iVideo.grab() && iVideo.retrieve(tFrame))
    {
        // PREPROCESS //

        // Convert to grayscale
        cv::Mat tFrameGray(tFrame.size(), CV_8U);
        cvtColor(tFrame, tFrameGray, CV_RGB2GRAY);

        // Sobel transform
        cv::Mat tFrameSobel(tFrame.size(), CV_16S);
        Sobel(tFrameGray, tFrameSobel, CV_16S, 3, 0, 9);

        // Convert to 32F
        cv::Mat tFrameSobelFloat(tFrame.size(), CV_8U);
        tFrameSobel.convertTo(tFrameSobelFloat, CV_32F, 1.0/256, 128);

        // Threshold
        cv::Mat tFrameThresholded = tFrameSobelFloat > 200;

        // Blank out useless region
        rectangle(tFrameThresholded, cv::Rect(0, 0, tFrame.size[1], tFrame.size[0] * 0.50), cv::Scalar::all(0), CV_FILLED);
        std::vector<cv::Point> tRectRight, tRectLeft;
        tRectRight.push_back(cv::Point(tFrame.size[1], tFrame.size[0]));
        tRectRight.push_back(cv::Point(tFrame.size[1]-tFrame.size[1]*0.25, tFrame.size[0]));
        tRectRight.push_back(cv::Point(tFrame.size[1], 0));
        fillConvexPoly(tFrameThresholded, &tRectRight[0], tRectRight.size(), cv::Scalar::all(0));
        tRectLeft.push_back(cv::Point(0, tFrame.size[0]));
        tRectLeft.push_back(cv::Point(0+tFrame.size[1]*0.25, tFrame.size[0]));
        tRectLeft.push_back(cv::Point(0, 0));
        fillConvexPoly(tFrameThresholded, &tRectLeft[0], tRectLeft.size(), cv::Scalar::all(0));

        // Final frame
        cv::Mat tFramePreprocessed = tFrameThresholded;


        // FEATURE DETECTION //

        // Frame with features
#if defined(DEBUG_PREPROCESSED)
        cv::Mat tFrameFeatures;
        cvtColor(tFramePreprocessed, tFrameFeatures, CV_GRAY2BGR);
#else
        cv::Mat tFrameFeatures = tFrame.clone();
#endif

        // Detect lines
        std::vector<cv::Vec4i> tLines;
        HoughLinesP(tFramePreprocessed, // Image
                    tLines,             // Lines
                    1,                  // Rho
                    CV_PI/180,          // Theta
                    20,                 // Threshold
                    50,                 // Minimum line length
                    3                   // Maximum line gap
                    );

        // Draw lines
        for(size_t i = 0; i < tLines.size(); i++)
        {
            line(tFrameFeatures,
                 cv::Point(tLines[i][0],
                           tLines[i][1]),
                 cv::Point(tLines[i][2],
                           tLines[i][3]),
                 cv::Scalar(0,0,255),
                 3,
                 8
                 );
        }

        // Find track start candidates
        std::vector<int> track_points, track_segments;
        int y = tFrame.size[0] - TRACK_START_OFFSET;
        for (int x = tFrame.size[1]-TRACK_WIDTH/2; x > TRACK_WIDTH/2; x--)
        {
            // Count the amount of segments intersecting with the current track start point
            int segments = 0;
            cv::Vec2i p1(x+TRACK_WIDTH/2, y);
            cv::Vec2i p2(x-TRACK_WIDTH/2, y);
            for(size_t i = 0; i < tLines.size(); i++)
            {
                cv::Vec2i p3(tLines[i][0], tLines[i][1]);
                cv::Vec2i p4(tLines[i][2], tLines[i][3]);

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
                    if (track_segments[i] < segments)
                    {
                        track_segments[i] = segments;
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
                    track_segments.push_back(segments);
                }
                else
                {
                    // Look for the track point with the least intersecting segments
                    int least = 0;
                    for (size_t i = 1; i < track_points.size(); i++)
                    {
                        if (track_segments[i] < track_segments[least])
                            least = i;
                    }

                    // Replace it
                    track_segments[least] = segments;
                    track_points[least] = x;
                }
            }
        }
        for (size_t i = 0; i < track_points.size(); i++)
        {
            if (track_segments[i] % 2)                                          // Small hack to improve detection
                track_points[i] -= TRACK_WIDTH/(2 * (track_segments[i] % 2));   // of the track center.
            int x = track_points[i];
            circle(tFrameFeatures, cv::Point(x, y), 5, cv::Scalar(0, 255, 255), -1);
        }

        // Detect track segments
        if (track_points.size() == 2)
        {
            int width = abs(track_points[0] - track_points[1]);
            if (width > TRACK_SPACE_MIN && width < TRACK_SPACE_MAX)
            {
                // Set the initial segment start
                std::vector<std::pair<cv::Point, cv::Point> > track_segments;
                track_segments.push_back(std::pair<cv::Point, cv::Point>(cv::Point(track_points[0]+1, y), cv::Point(track_points[1]+1, y)));

                // Detect new segments
                bool tNewSegment = true;
                while (tNewSegment)
                {
                    // Display latest segment
                    std::pair<cv::Point, cv::Point> segment_last = track_segments.back();
                    circle(tFrameFeatures, segment_last.first, 5, cv::Scalar(0, 255, 0), -1);
                    circle(tFrameFeatures, segment_last.second, 5, cv::Scalar(0, 255, 0), -1);
                    tNewSegment = false;

                    // Convenience variables for segment center
                    int x = segment_last.first.x;
                    int y = segment_last.first.y;

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
                            for(size_t i = 0; i < tLines.size(); i++)
                            {
                                // Line coordinates (inversly rotated)
                                cv::Point p1(tLines[i][0], tLines[i][1]);
                                cv::Point p1_rot(x + cos(-angle)*(p1.x-x) - sin(-angle)*(p1.y-y),
                                                 y + sin(-angle)*(p1.x-x) + cos(-angle)*(p1.y-y));
                                cv::Point p2(tLines[i][2], tLines[i][3]);
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
                                double slope_current = atan2(y_new - track_segments.back().first.y,
                                                             x_new - track_segments.back().first.x);
                                double slope_prev;
                                if (track_segments.size() >= 2)
                                {
                                    slope_prev = atan2(track_segments[track_segments.size()-1].first.y - track_segments[track_segments.size()-2].first.y,
                                                       track_segments[track_segments.size()-1].first.x - track_segments[track_segments.size()-2].first.x);
                                }
                                if (track_segments.size() == 1 || abs(slope_current - slope_prev) <= SEGMENT_ANGLE_DELTA_MAX)
                                {
                                    // Draw the segment
                                    cv::line(tFrameFeatures, r1_rot, r2_rot, cv::Scalar(0, 255, 0), 1);
                                    cv::line(tFrameFeatures, r2_rot, r3_rot, cv::Scalar(0, 255, 0), 1);
                                    cv::line(tFrameFeatures, r3_rot, r4_rot, cv::Scalar(0, 255, 0), 1);
                                    cv::line(tFrameFeatures, r4_rot, r1_rot, cv::Scalar(0, 255, 0), 1);

                                    // Save the segment
                                    track_segments.push_back(std::pair<cv::Point, cv::Point>(cv::Point(x_new, y_new), cv::Point(0, 0)));
                                    tNewSegment = true;
                                }
                            }

                            // Stop the current iteration
                            break;
                        }
                    }
                }
            }
        }


        // DISPLAY //

        // Display frame
        imshow("wp1", tFrameFeatures);
        if (oVideo.isOpened())
            oVideo << tFrameFeatures;

        // Halt on keypress
        if (cv::waitKey(30) >= 0)
            break;
    }

    return 0;
}
