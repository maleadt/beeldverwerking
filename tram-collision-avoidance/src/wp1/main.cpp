///////////////////////////////////////////////////////////////////////////////
// Configuration
//

// Headers
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <cmath>
#include <iostream>
#include <vector>


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


///////////////////////////////////////////////////////////////////////////////
// Main
//

int main(int argc, char** argv)
{
    //
    // Setup application
    //

    // Read command-line parameters
    if (argc != 2)
    {
        std::cout << "Error: invalid usage" << "\n";
        std::cout << "Usage: ./wp1 <filename>" << std::endl;
        return 1;
    }
    std::string iVideoFile = argv[1];

    // Open video
    cv::VideoCapture iVideo(iVideoFile);
    if(!iVideo.isOpened())
    {
        std::cout << "Error: could not open video" << std::endl;
        return 1;
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
#define DEBUG_PREPROCESSED
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
        const int track_width = 15;
        const int tracks = 2;
        std::vector<int> track_points, track_segments;
        int y = tFrame.size[0] - 10;
        for (int x = tFrame.size[1]-track_width/2; x > track_width/2; x--)
        {
            // Count the amount of segments intersecting with the current track start point
            int segments = 0;
            cv::Vec2i p1(x+track_width/2, y);
            cv::Vec2i p2(x-track_width/2, y);
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
                if ((track_points[i] - x) < track_width)
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
                if (track_points.size() < tracks)
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
                track_points[i] -= track_width/(2 * (track_segments[i] % 2));   // of the track center.
            int x = track_points[i];
            circle(tFrameFeatures, cv::Point(x, y), 5, cv::Scalar(0, 255, 255), -1);
        }

        // Detect track segments
        if (track_points.size() == 2)
        {
            int width = abs(track_points[0] - track_points[1]);
            if (width > 100 && width < 175)
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

                    // Scan
                    for (int length = 10; ; length += 10)
                    {
                        for (int angle = -45; angle < 45; angle += 5)
                        {
                        }
                        break;
                    }
                }
            }
        }


        // DISPLAY //

        // Display frame
        imshow("wp1", tFrameFeatures);

        // Halt on keypress
        if (cv::waitKey(30) >= 0)
            break;
    }

    return 0;
}
