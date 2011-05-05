//
// Configuration
//

// Includes
#include "pedestriandetection.h"

//
// Construction and destruction
//

PedestrianDetection::PedestrianDetection(const cv::Mat& iFrame) : Component(iFrame)
{
    std::string cascade_file("/home/ruben/haarcascade_fullbody.xml");
    cascade.load(cascade_file);
}


//
// Component interface
//

void PedestrianDetection::preprocess()
{

}

void PedestrianDetection::find_features(FrameFeatures& iFrameFeatures) throw(FeatureException)
{
    cropFrame();
    enhanceFrame();
    detectPedestrians(iFrameFeatures);
}
void PedestrianDetection::copy_features(const FrameFeatures& from, FrameFeatures& to) const
{
    to.pedestrians = from.pedestrians;
}

cv::Mat PedestrianDetection::frameDebug() const
{
    return mFrameDebug;
}


//
// Feature detection
//

void PedestrianDetection::cropFrame() {
    //VideoCapture cap("/media/windows/Beeldverwerking/HD/terug.MP4"); railsStartCol = 500; railsEndCol = 900;
    //VideoCapture cap("/media/windows/Beeldverwerking/HD/heen.MP4"); railsStartCol = 500; railsEndCol = 900;
    //VideoCapture cap("/media/windows/Beeldverwerking/obstakeldetectie.avi"); railsStartCol = 420; railsEndCol = 600;
    //VideoCapture cap("/media/windows/Beeldverwerking/tramdetectie.avi"); railsStartCol = 380; railsEndCol = 560;

    railsStartCol = 0;
    railsEndCol = frame().cols;

    cv::Range rowRange(0, frame().rows);
    cv::Range colRange(railsStartCol, railsEndCol);
    cv::Mat blockFromFrame(frame(), rowRange, colRange);

    //int scale = 3.5;
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

    //        imshow("image", dst);
    //        waitKey(0);
    //        imshow("image", frame);
    //        waitKey(0);
    //        return 0;
}
void PedestrianDetection::detectPedestrians(FrameFeatures& iFrameFeatures) {

    vector<cv::Rect> found_filtered;
    vector<cv::Rect> found;


    found_filtered.clear();
    //double t = (double)getTickCount();

    cascade.detectMultiScale(mFrameCropped, found);

    //t = (double)getTickCount() - t;
    //printf("tdetection time = %gms\n", t*1000./cv::getTickFrequency());

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
        r.x += railsStartCol;
        r.y *= scale;
        r.width *= scale;
        r.height *= scale;

        iFrameFeatures.pedestrians.push_back(r);

        //rectangle(frame, r.tl(), r.br(), cv::Scalar(0,0,255), 3);
    }

    //    Rect r;
    //    r.x = railsStartCol;
    //    r.width = railsEndCol - railsStartCol;
    //    r.y = 0;
    //    r.height = frame.rows;
    //    rectangle(frame, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
}
