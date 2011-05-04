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
#include "pedestriandetection.h"
#include "vehicledetection.h"

// Definitions
#define FEATURE_EXPIRATION 5

// Enumerations
enum Visualisation {
    FINAL = 1,
    DEBUG_TRACK,
};

// Enumeratie increment
template <class Enum> Enum & enum_increment(Enum& value, Enum begin, Enum end)
{
    return value = (value == end) ? begin : Enum(value + 1);
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
    cv::namedWindow("tram collision avoidance", 1);


    //
    // Process video
    //

    cv::Mat tFrame;
    unsigned int tFrameCount = 0;
    FrameFeatures tOldFeatures;
    unsigned int tAgeTrack = 0;
    Visualisation tVisualisationType = FINAL;
    unsigned int tVisualisationDuration = 0;
    while (iVideo.grab() && iVideo.retrieve(tFrame))
    {
        // Initialisation
        std::cout << "-- PROCESSING FRAME " << tFrameCount++ << " --" << std::endl;
        FrameFeatures tFeatures;

        // Manage visualisation
        cv::Mat tVisualisation;
        if (tVisualisationType == FINAL)
            tVisualisation = tFrame.clone();

        // Load objects
        TrackDetection tTrackDetection(tFrame);

        // Preprocess
        std::cout << "* Preprocessing" << std::endl;
        tTrackDetection.preprocess();
        if (tVisualisationType == DEBUG_TRACK)
            tVisualisation = tTrackDetection.frameDebug();

        // Find features
        std::cout << "* Finding features" << std::endl;
        try
        {
            // Find tracks
            std::cout << "- Finding tracks" << std::endl;
            try
            {
                tTrackDetection.find_features(tFeatures);
                tTrackDetection.copy_features(tFeatures, tOldFeatures);
                tAgeTrack = FEATURE_EXPIRATION;
            }
            catch (FeatureException e)
            {
                std::cout << "  Warning: " << e.what() << std::endl;
                if (tAgeTrack > 0)
                {
                    tAgeTrack--;
                    tTrackDetection.copy_features(tOldFeatures, tFeatures);
                }
                else
                    throw FeatureException("could not find the tracks");
            }

            // Draw tracks
            if (tVisualisationType == FINAL)
            {
                for (size_t i = 0; i < tFeatures.track_left.size()-1; i++)
                    cv::line(tVisualisation, tFeatures.track_left[i], tFeatures.track_left[i+1], cv::Scalar(0, 255, 0), 3);
                for (size_t i = 0; i < tFeatures.track_right.size()-1; i++)
                    cv::line(tVisualisation, tFeatures.track_right[i], tFeatures.track_right[i+1], cv::Scalar(0, 255, 0), 3);
            }
        }
        catch (FeatureException e)
        {
            std::cout << "! Error: " << e.what() << std::endl;
        }

        // Draw title
        if (tVisualisationDuration++ < 10)
        {
            // Generate visualisation title
            std::string tVisualisationTitle;
            switch (tVisualisationType)
            {
            case FINAL:
                tVisualisationTitle = "Final frame";
                break;
            case DEBUG_TRACK:
                tVisualisationTitle = "Track detection debug";
                break;
            default:
                tVisualisationTitle = "Unknown";
            }

            // Manage placing
            int tFontFace = cv::FONT_HERSHEY_DUPLEX;
            double tFontScale = 1;
            int tThickness = 1;
            int tBaseline = 0;
            cv::Size tTextSize = cv::getTextSize(tVisualisationTitle, tFontFace, tFontScale, tThickness, &tBaseline);
            cv::Point tTextOrigin(10, tTextSize.height + 10);
            putText(tVisualisation, tVisualisationTitle, tTextOrigin, tFontFace, tFontScale, cv::Scalar::all(255), tThickness, 8);
        }
        imshow("tram collision avoidance", tVisualisation);

        // Manage keypresses
        int tKeycode = cv::waitKey(30);
        switch (tKeycode)
        {
        case -1:
            break;
        case 32:
            enum_increment(tVisualisationType, FINAL, DEBUG_TRACK);
            tVisualisationDuration = 0;
            break;
        default:
            std::cout << tKeycode << std::endl;
            goto END;
        }
        std::cout << std::endl;
    }

    END:
        iVideo.release();

    return 0;
}

