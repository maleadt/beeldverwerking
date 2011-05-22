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

// Type definitions
typedef QPair<cv::Point, cv::Point> TrackStart;

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
    QList<Track > find_stitches(const QList<Line>& iRepresentatives);
    bool find_trackstart(const QList<Track>& iStitches, int iScanlineOffset, TrackStart& oTrackStart, QPair<Track, Track>& oTracks);
    bool is_valid(const QPair<Track, Track>& iOldTracks, QPair<Track, Track> iNewTracks);

    // Auxiliary methods
    bool groups_match(const QList<Line>& iGroupA, const QList<Line>& iGroupB);
    bool stitches_match(const Track& iStitchA, const Track& iStitchB, cv::Point& oIntersection);

    // Frames
    cv::Mat mFramePreprocessed;
    cv::Mat mFrameDebug;

    // Member data
    cv::RNG mRng;
};

#endif // TRACKDETECTION_H
