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
    adjustedX = 0;
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
    //Detecting current tracks width
    if (iFrameFeatures.tracks.first.length() > 1 && iFrameFeatures.tracks.second.length() > 1) {
        int x1 = iFrameFeatures.tracks.first[0].x;
        int y1 = iFrameFeatures.tracks.first[0].y;
        int x2 = iFrameFeatures.tracks.second[0].x;
        int y2 = iFrameFeatures.tracks.second[0].y;

        tracksWidth = sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
        tracksStartCol = x1;
        tracksEndCol = x2;
    }
    //Crop the frame = faster detection
    cropFrame();
    //Detect wheels (ellipse)
    detectWheels();
    //Detect cars from wheels
    detectVehiclesFromWheels(iFrameFeatures);
}

cv::Mat VehicleDetection::frameDebug() const
{
    return mFrameDebug;
}



//
// Feature detection
//
//Used so we don't have overlapping rects
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
    //Only interested in the area next to the tracks
    if (tracksWidth > -1) {
        adjustedX = tracksStartCol - 1.2*tracksWidth;
        if (adjustedX < 0) {
            adjustedX = 0;
        }
        cv::Range rowRange(0, frame()->rows);
        cv::Range colRange(adjustedX, (tracksEndCol + 1.2*tracksWidth > frame()->cols?frame()->cols:tracksEndCol + 1.2*tracksWidth));
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
    for (int i = start; i < end; i += 15) {
        //Find all contours
        std::vector<std::vector<cv::Point> > contours;
        cv::Mat bimage = img >= i;

        findContours(bimage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1 );

        for(size_t i = 0; i < contours.size(); i++)
        {
            //Check if the ellipse found is valid (not too big/small etc)
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
            cv::Point p = box.center;
            p.x += adjustedX;
            //Draw ellipse on debug
            cv::ellipse(mFrameDebug, p, box.size*0.5f, box.angle, 0, 360, cv::Scalar(0,255,255), 1, CV_AA);

            //Check if it does not overlap any existing wheel (that's not possible!)
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
    //Save the found wheels
    std::list<Rectangle*>::iterator it;
    for ( it=lst.begin() ; it != lst.end(); it++ ) {
        if ((*it) != 0) {
            cv::Rect r = (*it)->getRect();
            r.x += adjustedX;
            vehicles.push_back(r);
        }
    }
}
void VehicleDetection::detectVehiclesFromWheels(FrameFeatures& iFrameFeatures) {
    bool added = false;
    std::vector<int> connected;
    std::vector<int> weights;

    connected.resize(vehicles.size(), -1);
    weights.resize(vehicles.size(), -1);

    int maxCar;
    if (tracksWidth > -1) { //Only detect vehicles if we have a reference from the tracks!
        maxCar = tracksWidth * 3;

        for (size_t i = 0; i < vehicles.size(); i++) {
            for (size_t j = 0; j < vehicles.size(); j++) {
                if (i != j) {

                    //Distance between i & j:
                    cv::Point iPoint(vehicles[i].x + vehicles[i].width/2, vehicles[i].y+vehicles[i].height/2);
                    cv::Point jPoint(vehicles[j].x + vehicles[j].width/2, vehicles[j].y+vehicles[j].height/2);
                    int distance = sqrt(pow(jPoint.x - iPoint.x,2) + pow(jPoint.y - jPoint.x,2));

                    //Check if distance is allowed & better then previous ones
                    if (distance < maxCar) {
                        if (weights[i] == -1 || distance < weights[i]) {
                            weights[i] = distance;
                            weights[j] = distance;

                            if (connected[i] > -1) {
                                connected[connected[i]] = -1;
                                connected[i] = -1;
                            }

                            connected[i] = j;
                            connected[j] = i;
                        }
                    }
                }
            }
        }

        //Create cars!
        for (size_t i = 0; i < connected.size(); i++) {
            if (connected[i] > -1) {
                int x1 = vehicles[i].x;
                int y1 = vehicles[i].y;
                int w1 = vehicles[i].width;

                int x2 = vehicles[connected[i]].x;
                int y2 = vehicles[connected[i]].y;
                int w2 = vehicles[connected[i]].width;

                int x = (x1 < x2?x1:x2);
                int y = (y1<y2?y1:y2);
                int height = (vehicles[i].height > vehicles[connected[i]].height?vehicles[i].height:vehicles[connected[i]].height);
                if (height < 1.5*tracksWidth) { //Car can't be smaller then this!
                    y -= 1.5*tracksWidth - height;
                    height = 1.5*tracksWidth;
                }
                int width = (x1<x2?x2+w2:x1+w1) - (x1<x2?x1:x2);

                cv::Rect r(x, y,width,height);

                if (!added) { //Remove previous found!
                    iFrameFeatures.vehicles.clear();
                }

                iFrameFeatures.vehicles.push_back(r);

                added = true;
                //std::cout<<i<<" met "<<connected[i]<<std::endl;
            }
        }
    }
    //If no features are found: exception
    if (!added) {
        throw FeatureException("no vehicles found");
    }
}


