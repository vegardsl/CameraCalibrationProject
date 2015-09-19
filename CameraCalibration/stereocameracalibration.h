#ifndef STEREOCAMERACALIBRATION_H
#define STEREOCAMERACALIBRATION_H

#include <QtCore>

#include "stdio.h"
#include "stdlib.h"
#include <cstdint>
#include <iomanip>


#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <globalparameters.h>
#include "videostreamthread.h"

using namespace cv;
using namespace std;

class StereoCameraCalibration : public QObject
{
    Q_OBJECT

private:
    QString l_path, r_path; //path for folders containing left and right checkerboard images
    QDir appDir;
    vector<Mat> l_images, r_images; //left and right checkerboard images
    Mat l_cameraMatrix, l_distCoeffs, r_cameraMatrix, r_distCoeffs; //Mats for holding individual camera calibration information
    bool show_chess_corners; //visualize checkerboard corner detections?
    float side_length; //side length of checkerboard squares
    int width, height; //number of internal corners in checkerboard along width and height
    vector<vector<Point2f> > l_image_points, r_image_points; //left and right image points
    vector<vector<Point3f> > object_points; //object points (grid)
    Mat R, T, E, F; //stereo calibration information
    void getStoredSamples(QString side);
    void pushBackSample(string file, QString side);
    void generateCalibrationStereoSamples(QDir l_dir, QDir r_dir);

    VideoStreamThread leftVideoStreamThread, rightVideoStreamThread;
    Mat leftCameraFeed, rightCameraFeed, l_imshow, r_imshow, l_gray, r_gray;

public:
    StereoCameraCalibration::StereoCameraCalibration();
    void initStereoCameraCalibration(QDir, QString, QString, float, int, int);//constructor
    bool calibrate(); //function to calibrate stereo camera
    void calc_image_points(bool); //function to calculae image points by detecting checkerboard corners

signals:

public slots:
};

#endif // STEREOCAMERACALIBRATION_H
