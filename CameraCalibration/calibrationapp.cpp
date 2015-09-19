#include "calibrationapp.h"

#include "globalparameters.h"

#include "singlecamcalibrator.h"
#include "stereocameracalibration.h"
#include "stereoaligner.h"
#include "videostreamthread.h"
#include "disparitycalcthread.h"

#define CALIB_DATA_FOLDER "C:\\Users\\vegarsl\\Pictures\\StereoSamples\\Calibration\\Data\\right\\"



using namespace std;

CalibrationApp::CalibrationApp()
{
    onCreate();

    getchar();
}

void CalibrationApp::onCreate(){

    //QDir directory("C:/Users/vegarsl/Pictures/StereoSamples/Calibration/Data/left/");

    const string videoStreamAddress[] = { "http://admin:admin@192.168.0.100:80/video.mjpg", "http://admin:admin@192.168.0.102:80/video.mjpg" };

    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    //string sampleFilePath = "C:\\Users\\vegarsl\\Pictures\\StereoSamples\\Calibration\\Data\\left\\";
    float sideLength = 26.f;
    int width = 9;
    int height = 6;

    QDir dir;
    dir.mkpath("samples/single/left/");
    dir.mkpath("samples/single/right/");
    dir.mkpath("samples/stereo/left/");
    dir.mkpath("samples/stereo/right/");
    qDebug() << dir.absolutePath();
    QString appPath = dir.absolutePath();
/*
  // Calibration sequence for single cam.
    SingleCamCalibrator l_singleCamCalibrator;
    l_singleCamCalibrator.initSingleCamCalibrator("left",dir, sideLength, width, height);
    l_singleCamCalibrator.calc_image_points(true);
    l_singleCamCalibrator.calibrate();
    string l_filename = "samples/single/left/" + string("cam_calib.xml");
    FileStorage l_fs(l_filename, FileStorage::WRITE);
    l_fs << "cameraMatrix" << l_singleCamCalibrator.get_cameraMatrix();
    l_fs << "distCoeffs" << l_singleCamCalibrator.get_distCoeffs();
    l_fs.release();
    cout << "Saved calibration matrices to " << l_filename << endl;

    // Calibration sequence for single cam.
    SingleCamCalibrator r_singleCamCalibrator;
    r_singleCamCalibrator.initSingleCamCalibrator("right",dir, sideLength, width, height);
    r_singleCamCalibrator.calc_image_points(true);
    r_singleCamCalibrator.calibrate();
    string r_filename = "samples/single/right/" + string("cam_calib.xml");
    FileStorage r_fs(r_filename, FileStorage::WRITE);
    r_fs << "cameraMatrix" << r_singleCamCalibrator.get_cameraMatrix();
    r_fs << "distCoeffs" << r_singleCamCalibrator.get_distCoeffs();
    r_fs.release();
    cout << "Saved calibration matrices to " << r_filename << endl;

*/
/*
   // Calibration sequence for stereo calibration.
    StereoCameraCalibration stereoCameraCalibration;
    stereoCameraCalibration.initStereoCameraCalibration(dir,"samples/stereo/left/","samples/stereo/right/",sideLength, width, height);
    stereoCameraCalibration.calc_image_points(true);
    bool done = stereoCameraCalibration.calibrate();
    if(!done) cout << "Stereo Calibration not successful because individial calibration matrices could not be read" << endl;


    //Size image_size = stereoCameraCalibration.get_image_size();


    StereoAligner mStereoAligner;
    Size image_size(1280, 720);
    mStereoAligner.align(appPath,image_size);

    mStereoAligner.show_aligned(image_size);

*/

    Size image_size(1280, 720);
    //VideoStreamThread videoRight, videoLeft;
    DisparityCalcThread disparityCalcThread;

    //videoLeft.beginVideoStream(videoStreamAddress[0],"left");
    //videoRight.beginVideoStream(videoStreamAddress[1],"right");
    disparityCalcThread.beginDisparityCalcThread(appPath,image_size);

    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<string>("string");

    QObject::connect(&videoLeft, SIGNAL(frameReady(cv::Mat,string)),
                     &disparityCalcThread, SLOT(newFrameForProcessing(cv::Mat, string))
                     );

    QObject::connect(&videoRight, SIGNAL(frameReady(cv::Mat,string)),
                     &disparityCalcThread, SLOT(newFrameForProcessing(cv::Mat, string))
                     );




    //const string videoStreamAddress[] = { "http://admin:admin@192.168.0.100:80/video.mjpg", "http://admin:admin@192.168.0.102:80/video.mjpg" };

    //videoLeft.beginVideoStream(videoStreamAddress[0], "Left Camera");
    //videoRight.beginVideoStream(videoStreamAddress[1], "Right Camera");

    forever{
        getchar();
        qDebug() << "foo";
    }
}
