//
// Configuration
//

// Include guard
#ifndef TRACKDETECTION_H
#define TRACKDETECTION_H

// Includes
#include "opencv/cv.h"
#include <cmath>
#include <QVector>
#include <QPair>
#include "component.h"
#include "framefeatures.h"

typedef QPair<cv::Point, cv::Point> Line;

class TrackDetection : public Component
{
public:
    // Construction and destruction
    TrackDetection(cv::Mat const* iFrame);

    // Component interface
    void preprocess();
    void find_features(FrameFeatures& iFrameFeatures) throw(FeatureException);
    cv::Mat frameDebug() const;

private:
    // Feature detection
    QList<Line > find_lines();
    QList<QList<Line> > find_groups(const QList<Line>& iLines);
    QList<Line> find_representatives(const QList<QList<Line> >& iGroups);
    QList<QList<cv::Point> > find_stitches(const QList<Line>& iRepresentatives);

    // Auxiliary math
    double distance_segment2segment(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersectA, cv::Point& oIntersectB) const;
    double distance_point2segment(cv::Point iPoint, Line iSegment, cv::Point& oIntersect) const;
    bool intersect_segments(const Line& iSegmentA, const Line& iSegmentB, cv::Point& oIntersect) const;
    bool groups_match(const QList<Line>& iGroupA, const QList<Line>& iGroupB);
    bool stitches_match(const QList<cv::Point>& iStitchA, const QList<cv::Point>& iStitchB, cv::Point& oIntersection);

    // Frames
    cv::Mat mFramePreprocessed;
    cv::Mat mFrameDebug;

    // Member data
    cv::RNG mRng;
};

#endif // TRACKDETECTION_H
