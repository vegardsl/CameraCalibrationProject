#ifndef DISPARITYCALCTHREAD_H
#define DISPARITYCALCTHREAD_H

#include <QtCore>
#include <QDebug>

#include <cstdint>
#include <iomanip>

#include "stdio.h"
#include "stdlib.h"


#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/cuda.hpp>
#include <C:\OpenCV\sources\modules\cudastereo\include\opencv2\cudastereo.hpp>


#include "videostreamthread.h"

using namespace cv;
using namespace std;

class DisparityCalcThread : public QThread
{
    Q_OBJECT

private:
    Mat map_l1, map_l2, map_r1, map_r2; // rectification pixel maps
    //StereoSGBM stereo; // stereo matching object for disparity computation
    int min_disp, num_disp; // parameters of StereoSGBM

    Mat leftFrame, rightFrame, leftGray, rightGray, framel_rect, framer_rect;
    Mat disp16S, disp8U;
    //-- Extreme values of disparity image.
    double minVal, maxVal;

    int bothFramesArrived = false;

    //QSemaphore leftFrameArrived;
    //QSemaphore rightFrameArrived;

    VideoStreamThread videoRight, videoLeft;

    /* --- CUDA declarations --- */
    cuda::GpuMat l_disp, r_disp, gpu_disp;

public:
    DisparityCalcThread(QObject *parent = 0);

    void beginDisparityCalcThread(QString, Size);

    //void set_minDisp(int minDisp) { stereo.minDisparity = minDisp; }
    //void set_numDisp(int numDisp) { stereo.numberOfDisparities = numDisp; }
    void show_disparity(Size); // show live disparity by processing stereo camera feed

protected:

    void run() Q_DECL_OVERRIDE;

signals:

public slots:
    void newFrameForProcessing(cv::Mat frame, string frameSide);
};

#endif // DISPARITYCALCTHREAD_H
