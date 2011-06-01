//
// Configuration
//

// Include guard
#ifndef AUXILIARY_H
#define AUXILIARY_H

// Includes
#include "opencv/cv.h"


//
// Geometry
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

// Test of two segments intersect
bool intersect(const cv::Point &p1, const cv::Point &p2,
               const cv::Point &p3, const cv::Point &p4)
{
    return     (twz(p1, p2, p3) * twz(p1, p2, p4) <= 0)
            && (twz(p3, p4, p1) * twz(p3, p4, p2) <= 0);
}

// Fetch the intersection point of two colliding segments (this has to
// be tested beforehand)
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


#endif // AUXILIARY_H
