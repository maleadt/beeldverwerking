///////////////////////////////////////////////////////////////////////////////
// Configuration
//

// Headers
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <iostream>
#include "framefeatures.h"
#include "component.h"
#include "trackdetection.h"


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
    cv::namedWindow("tram-collision-avoidance", 1);


    //
    // Process video
    //

    cv::Mat tFrame;
    while (iVideo.grab() && iVideo.retrieve(tFrame))
    {
        // Set-up struct with features
        FrameFeatures tFrameFeatures;

        // Detect tracks
        TrackDetection tTrackDetection(tFrame);
        tTrackDetection.preprocess();
#if defined(DEBUG_PREPROCESSED)
        tTrackDetection.setFrameDebug(tTrackDetection.framePreprocessed());
        cvtColor(tFramePreprocessed, tFrameFeatures, CV_GRAY2BGR);
#else
        tTrackDetection.setFrameDebug(tFrame.clone());
#endif
        tTrackDetection.find_features(tFrameFeatures);

        // Draw debug screen
        // TODO: draw features instead of debug
        imshow("track detection", tTrackDetection.frameDebug());

        // Halt on keypress
        if (cv::waitKey(30) >= 0)
            break;
    }

    return 0;
}
