#ifndef SINGLECAMCALIBRATOR_H
#define SINGLECAMCALIBRATOR_H

// Program illustrate single camera calibration
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <QtCore>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include <cstdint>
#include <iomanip>

#include "stdio.h"
#include "stdlib.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <globalparameters.h>
#include "videostreamthread.h"


using namespace cv;
using namespace std;

class SingleCamCalibrator : public QObject{

     Q_OBJECT

private:
    QString selectedCamera;
    QDir appDirPath;                            //path of folder containing chessboard images
    QDir sampleFilePath;
    vector<Mat> images;                             //chessboard images
    Mat cameraMatrix, distCoeffs;                   //camera matrix and distortion coefficients
    bool show_chess_corners;                        //visualize the extracted chessboard corners?
    double squareSideLength;                         //side length of a chessboard square in mm
    int width_internalCorners, height_internalCorners; //number of internal corners of the chessboard along width and height

    vector<vector<Point2f> > image_points;          //2D image points
    vector<vector<Point3f> > object_points;         //3D object points

    void generateCalibrationSamples(QDir sampleFilePath, QString cameraSelection);
    QSemaphore waitForFirstFrame;

    Mat cameraFeed;
    VideoStreamThread mVideoStreamThread;

public:
    SingleCamCalibrator::SingleCamCalibrator();
    int initSingleCamCalibrator(QString selectedCamera,
                                QDir appDirPath,
                                double _squareSideLength,
                                int _width_internalCorners,
                                int _height_internalCorners);   //constructor, reads in the images
    void calibrate();                                   //function to calibrate the camera
    Mat get_cameraMatrix();                             //access the camera matrix
    Mat get_distCoeffs();                               //access the distortion coefficients
    void calc_image_points(bool);                       //calculate internal corners of the chessboard image

signals:

public slots:

};

#endif // SINGLECAMCALIBRATOR_H
