#ifndef STEREOALIGNER_H
#define STEREOALIGNER_H

// Program illustrate stereo camera rectification
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <QtCore>
#include <QDebug>

#include <cstdint>
#include <iomanip>

#include "stdio.h"
#include "stdlib.h"


#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "videostreamthread.h"

using namespace cv;
using namespace std;

class StereoAligner
{
private:
    Mat map_l1, map_l2, map_r1, map_r2; //pixel maps for rectification
    string path;

    VideoStreamThread leftVideoStreamThread, rightVideoStreamThread;
    Mat leftCameraFeed, rightCameraFeed, leftCameraFeed_rect, rightCameraFeed_rect;

public:
    StereoAligner::StereoAligner(); //constructor
    void align(QString, Size);
    void show_aligned(Size); //function to show live rectified feed from stereo camera

signals:

public slots:
};

#endif // STEREOALIGNER_H
