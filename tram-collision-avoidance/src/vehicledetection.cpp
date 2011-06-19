//
// Configuration
//

// Includes
#include "vehicledetection.h"

//Feature properties
#define VEHICLE_sliderPos 35

#define VEHICLE_LOW_BOUND 15
#define VEHICLE_HIGH_BOUND 200


//
// Construction and destruction
//

VehicleDetection::VehicleDetection(cv::Mat const* iFrame) : Component(iFrame)
{
    tracksWidth = -1;
}


//
// Component interface
//

void VehicleDetection::preprocess()
{
    mFrameDebug = (*frame()).clone();
}

void VehicleDetection::find_features(FrameFeatures& iFrameFeatures) throw(FeatureException)
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
    detectWheels();
    for (size_t i = 0; i < vehicles.size(); i++) {
        iFrameFeatures.vehicles.push_back(vehicles[i]);
    }
}

cv::Mat VehicleDetection::frameDebug() const
{
    return mFrameDebug;
}



//
// Feature detection
//
class Rectangle {
public:
    Rectangle(cv::RotatedRect _box) {
        this->center = _box.center;
        this->tl = _box.boundingRect().tl();
        this->br = _box.boundingRect().br();
    }
    void draw(cv::Mat &img) {
        line(img, this->tl, cv::Point2f(this->br.x, this->tl.y), cv::Scalar(0,255,0), 1, CV_AA);
        line(img, cv::Point2f(this->br.x, this->tl.y), this->br, cv::Scalar(0,255,0), 1, CV_AA);
        line(img, this->tl, cv::Point2f(this->tl.x, this->br.y), cv::Scalar(0,255,0), 1, CV_AA);
        line(img, cv::Point2f(this->tl.x, this->br.y), this->br, cv::Scalar(0,255,0), 1, CV_AA);
    }
    cv::Point2f getCenter() {
        return this->center;
    }
    bool contains(Rectangle* r) {
        cv::Point2f c = (*r).getCenter();
        if (c.x > this->tl.x && c.x < this->br.x) {
            if (c.y > this->tl.y && c.y < this->br.y) {
                return true;
            }
        }
        return false;
    }
    bool isBigger(Rectangle * r) {
        return (this->getArea() > r->getArea());
    }
    float getArea() {
        return (br.x - tl.x) * (tl.y - br.y);
    }
    cv::Rect getRect() {
        return cv::Rect(tl.x, tl.y, br.x-tl.x, br.y-tl.y);
    }

private:
    cv::Point2f center;
    cv::Point2f tl;
    cv::Point2f br;
};

void VehicleDetection::cropFrame() {
    if (tracksWidth > -1) {
        adjustedX = tracksStartCol - 1.5*tracksWidth;
        if (adjustedX < 0) {
            adjustedX = 0;
        }
        cv::Range rowRange(0, frame()->rows);
        cv::Range colRange(adjustedX, (tracksEndCol + 2*tracksWidth > frame()->cols?frame()->cols:tracksEndCol + 1.5*tracksWidth));
        mFrameCropped = cv::Mat(*frame(), rowRange, colRange);
    } else {
        mFrameCropped = mFrameDebug.clone();
    }
}

void VehicleDetection::detectWheels() {



    cv::Mat img;
    cv::cvtColor(mFrameCropped, img, CV_RGB2GRAY);

    std::list<Rectangle*> lst;
    lst.resize(50, 0);

    int start = 5; int end = 100;
    for (int i = start; i < end; i += 15) { //i += 5
        std::vector<std::vector<cv::Point> > contours;
        cv::Mat bimage = img >= i;

        findContours(bimage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1 );

        for(size_t i = 0; i < contours.size(); i++)
        {
            size_t count = contours[i].size();
            if( count < 6 )
                continue;

            cv::Mat pointsf;
            cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
            cv::RotatedRect box = cv::fitEllipse(pointsf);


            cv::Point2f vtx[4];
            box.points(vtx);

            int height = box.boundingRect().br().y - box.boundingRect().tl().y;
            int width = box.boundingRect().br().x - box.boundingRect().tl().x;

            if (height < width*.92)
                continue;

            if( box.size.height < box.size.width)
                continue;

            if (MIN(box.size.width, box.size.height) < VEHICLE_LOW_BOUND || MAX(box.size.width, box.size.height) > VEHICLE_HIGH_BOUND)
                continue;

            if (tracksWidth > -1) {
                if (width < tracksWidth / 3 || width > tracksWidth/3*2) {
                    continue;
                }
                if (height < tracksWidth / 3 || height > tracksWidth/3*2) {
                    continue;
                }
            }

            cv::ellipse(mFrameDebug, box.center, box.size*0.5f, box.angle, 0, 360, cv::Scalar(0,255,255), 1, CV_AA);

            Rectangle * r = new Rectangle(box);

            std::list<Rectangle*>::iterator it;
            bool add = true;
            it=lst.begin();
            while (it != lst.end()) {
                if ((*it) != 0) {
                    if ((*(*it)).contains(r)) {
                        if (r->isBigger(*it)) { //add r, remove *it
                            it = lst.erase(it);
                            continue;
                        } else {
                            add = false;
                        }
                    }
                }
                it++;
            }
            if (add) {
                lst.push_back(r);
            }
        }
    }
    std::list<Rectangle*>::iterator it;
    for ( it=lst.begin() ; it != lst.end(); it++ ) {
        if ((*it) != 0) {
            cv::Rect r = (*it)->getRect();
            r.x += adjustedX;
            vehicles.push_back(r);
        }
    }
}


