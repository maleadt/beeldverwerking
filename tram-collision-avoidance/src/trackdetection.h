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
#include "auxiliary.h"

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

    // Auxiliary methods
    bool groups_match(const QList<Line>& iGroupA, const QList<Line>& iGroupB);
    bool stitches_match(const QList<cv::Point>& iStitchA, const QList<cv::Point>& iStitchB, cv::Point& oIntersection);

    // Frames
    cv::Mat mFramePreprocessed;
    cv::Mat mFrameDebug;

    // Member data
    cv::RNG mRng;
};

#endif // TRACKDETECTION_H
