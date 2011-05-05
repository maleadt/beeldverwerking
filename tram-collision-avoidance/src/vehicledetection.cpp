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

VehicleDetection::VehicleDetection(const cv::Mat& iFrame) : Component(iFrame)
{

}


//
// Component interface
//

void VehicleDetection::preprocess()
{
    detectWheels();
}

void VehicleDetection::find_features(FrameFeatures& iFrameFeatures) throw(FeatureException)
{
    for (size_t i = 0; i < vehicles.size(); i++) {
        iFrameFeatures.vehicles.push_back(vehicles[i]);
    }
}
void VehicleDetection::copy_features(const FrameFeatures& from, FrameFeatures& to) const
{
    to.vehicles = from.vehicles;
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
        //this->box = _box;
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
    //RotatedRect box;
};


void VehicleDetection::detectWheels() {

    cv::Mat img;
    cv::cvtColor(frame(), img, CV_RGB2GRAY);

    std::list<Rectangle*> lst;
    lst.resize(50, 0);

    int start = 5; int end = 100;
    for (int i = start; i < end; i += 15) { //i += 5
        std::vector<std::vector<cv::Point> > contours;
        cv::Mat bimage = img >= i;

        findContours(bimage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE ); //CV_RETR_LIST

        cv::Mat cimage = cv::Mat::zeros(bimage.size(), CV_8UC3);

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
            //std::cout<<box.boundingRect().tl()<<std::endl;

            if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*2 )
                continue;

            if (MIN(box.size.width, box.size.height) < VEHICLE_LOW_BOUND || MAX(box.size.width, box.size.height) > VEHICLE_HIGH_BOUND)
                continue;

            //ellipse(frame, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0,255,255), 1, CV_AA);


            //std::cout << std::endl;
            /*for( int j = 0; j < 4; j++ ) {
                line(frame, vtx[j], vtx[(j+1)%4], Scalar(0,255,0), 1, CV_AA);
                //std::cout<<vtx[j]<<" ";
            }*/

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
            vehicles.push_back((*(*it)).getRect());
            //(*(*it)).draw(frame);
        }
    }
}


