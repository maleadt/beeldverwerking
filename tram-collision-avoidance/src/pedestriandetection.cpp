//
// Configuration
//

// Includes
#include "pedestriandetection.h"

//
// Construction and destruction
//

PedestrianDetection::PedestrianDetection(cv::Mat const* iFrame) : Component(iFrame)
{
    std::string cascade_file("/home/ruben/haarcascade_fullbody.xml");
    cascade.load(cascade_file);
}


//
// Component interface
//

void PedestrianDetection::preprocess()
{
    mFrameDebug = (*frame()).clone();
}

void PedestrianDetection::find_features(FrameFeatures& iFrameFeatures) throw(FeatureException)
{
    if (iFrameFeatures.tracks.first.length() > 1 && iFrameFeatures.tracks.second.length() > 1) {
        int x1 = iFrameFeatures.tracks.first[0].x;
        int y1 = iFrameFeatures.tracks.first[0].y;
        int x2 = iFrameFeatures.tracks.second[0].x;
        int y2 = iFrameFeatures.tracks.second[0].y;

        tracksWidth = sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
        tracksStartCol = x1;
        tracksEndCol = x2;
    }

    cropFrame();
    enhanceFrame();
    detectPedestrians(iFrameFeatures);
}

cv::Mat PedestrianDetection::frameDebug() const
{
    return mFrameDebug;
}


//
// Feature detection
//

void PedestrianDetection::cropFrame() {
    cv::Range rowRange(0, frame()->rows);
    adjustedX = tracksStartCol - 2*tracksWidth;
    if (adjustedX < 0) {
        adjustedX = 0;
    }
    cv::Range colRange(adjustedX, (tracksEndCol + 2*tracksWidth > frame()->cols?frame()->cols:tracksEndCol + 2*tracksWidth));
    cv::Mat blockFromFrame(*frame(), rowRange, colRange);

    scale = blockFromFrame.rows / 190;
    mFrameCropped = cv::Mat(blockFromFrame.rows / scale, blockFromFrame.cols / scale, CV_8UC3);
    cv::resize(blockFromFrame, mFrameCropped, mFrameCropped.size(), 0, 0, cv::INTER_LINEAR);
}
void PedestrianDetection::enhanceFrame() {
    //    int brightness = 0;
    //        int contrast = 0;
    //        double a, b;
    //        if( contrast > 0 )
    //        {
    //            double delta = 127.*contrast/100;
    //            a = 255./(255. - delta*2);
    //            b = a*(brightness - delta);
    //        }
    //        else
    //        {
    //            double delta = -128.*contrast/100;
    //            a = (256.-delta*2)/255.;
    //            b = a*brightness + delta;
    //        }
    //        Mat dst;
    //        frame.convertTo(dst, CV_8U, a, b);
    //        GaussianBlur(dst, dst, Size(5, 5), 1.2, 1.2);
}
void PedestrianDetection::detectPedestrians(FrameFeatures& iFrameFeatures) {

    std::vector<cv::Rect> found_filtered;
    std::vector<cv::Rect> found;


    found_filtered.clear();

    cascade.detectMultiScale(mFrameCropped, found);

    size_t j;
    for(size_t i = 0; i < found.size(); i++ )
    {
        cv::Rect r = found[i];
        for( j = 0; j < found.size(); j++ )
            if( j != i && (r & found[j]) == r)
                break;
        if( j == found.size() )
            found_filtered.push_back(r);
    }

    for(size_t i = 0; i < found_filtered.size(); i++ )
    {
        cv::Rect r = found_filtered[i];
        r.x *= scale;
        r.x += adjustedX;
        r.y *= scale;
        r.width *= scale;
        r.height *= scale;

        iFrameFeatures.pedestrians.push_back(r);

        cv::rectangle(mFrameDebug, r.tl(), r.br(), cv::Scalar(0,0,255), 2);
    }
}
