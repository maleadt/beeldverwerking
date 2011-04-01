#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>

using namespace cv;
using namespace std;

void help()
{
    printf(
            "\nDemonstrate the use of the HoG descriptor using\n"
            "  HOGDescriptor::hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());\n"
            "Usage:\n"
            "./peopledetect (<image_filename> | <image_list>.txt)\n\n");
}

String cascadeName = "/home/ruben/OpenCV-2.2.0/data/haarcascades/haarcascade_fullbody.xml";
//String cascadeName = "/home/ruben/haarcascade_fullbody.xml";
String imageName = "/media/windows/test-images/1.png";

CvHaarClassifierCascade* load_object_detector( const char* cascade_path )
{
    return (CvHaarClassifierCascade*)cvLoad( cascade_path );
}

void detect_and_draw_objects( IplImage* image,
                              CvHaarClassifierCascade* cascade,
                              int do_pyramids )
{
    IplImage* small_image = image;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* faces;
    int i, scale = 1;

    /* if the flag is specified, down-scale the è¾“å…¥å›¾åƒ to get a
       performance boost w/o loosing quality (perhaps) */
    if( do_pyramids )
    {
        small_image = cvCreateImage( cvSize(image->width/2,image->height/2),
                                     IPL_DEPTH_8U, 3 );
        cvPyrDown( image, small_image, CV_GAUSSIAN_5x5 );
        scale = 2;
    }

    /* use the fastest variant */
    double t = (double)getTickCount();
    faces = cvHaarDetectObjects( small_image, cascade, storage,  1.05,2, CV_HAAR_DO_CANNY_PRUNING );
    t = (double)getTickCount() - t;
    printf("tdetection time = %gms\n", t*1000./cv::getTickFrequency());

    /* draw all the rectangles */
    for( i = 0; i < faces->total; i++ )
    {
        /* extract the rectanlges only */
        CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i );
        cvRectangle( image, cvPoint(face_rect.x*scale,face_rect.y*scale),
                     cvPoint((face_rect.x+face_rect.width)*scale,
                             (face_rect.y+face_rect.height)*scale),
                     CV_RGB(255,0,0), 3 );
    }

    if( small_image != image )
        cvReleaseImage( &small_image );
    cvReleaseMemStorage( &storage );
}
template <typename T>
        std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

int main(int argc, char** argv)
{
    std::string img_folder("/media/windows/test-images/");
    std::string cascade_file("/home/ruben/OpenCV-2.2.0/data/haarcascades/haarcascade_fullbody.xml");
    CascadeClassifier cascade(cascade_file);
    int total = 0;
    //for (int i = 1; i <= 14; i++) {

    //VideoCapture cap("/media/windows/Beeldverwerking/HD/terug.MP4");
    VideoCapture cap("/media/windows/Beeldverwerking/HD/heen.MP4");
    //VideoCapture cap("/media/windows/Beeldverwerking/obstakeldetectie.avi");
    //VideoCapture cap("/media/windows/Beeldverwerking/tramdetectie.avi");
    if(!cap.isOpened())
        return -1;

    int cnt = 0;
    vector<Rect> found_filtered;
    for (;;) {
        cnt++;
        cnt %= 5;
        Mat frame;

        vector<Rect> found;

        /*std::string img_file(img_folder);
        img_file.append(to_string(i)).append(".png");
        frame = imread(img_file);*/
        cap>>frame;

        int scale = 3.5;
        Mat img = cv::Mat(frame.rows / scale, frame.cols / scale, CV_8UC3);
        cv::resize(frame, img, img.size(), 0, 0, INTER_LINEAR);

        if (cnt == 0) {
            found_filtered.clear();
            double t = (double)getTickCount();

            cascade.detectMultiScale(img, found);

            t = (double)getTickCount() - t;
            printf("tdetection time = %gms\n", t*1000./cv::getTickFrequency());

            printf("Found: %d\n", found.size());
            total += found.size();
            size_t j;
            for(size_t i = 0; i < found.size(); i++ )
            {
                Rect r = found[i];
                for( j = 0; j < found.size(); j++ )
                    if( j != i && (r & found[j]) == r)
                        break;
                if( j == found.size() )
                    found_filtered.push_back(r);
            }
        }
        for(size_t i = 0; i < found_filtered.size(); i++ )
        {
            Rect r = found_filtered[i];
            r.x *= scale;
            r.y *= scale;
            r.width *= scale;
            r.height *= scale;
            rectangle(frame, r.tl(), r.br(), cv::Scalar(0,0,255), 3);
        }

        imshow("people detector", frame);
        if(waitKey(30) >= 0)
            break;
    }

    printf("total found: %d\n", total);
    return 0;
}
