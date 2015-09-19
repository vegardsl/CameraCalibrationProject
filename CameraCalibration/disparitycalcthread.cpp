#include "disparitycalcthread.h"

DisparityCalcThread::DisparityCalcThread(QObject *parent) : QThread(parent)
{

}

void DisparityCalcThread::newFrameForProcessing(Mat frame, string frameSide)
{
/*
    if(frameSide == "Left Camera"){
        //framesArrived.aquire(1);
        leftFrame = frame;
        cvtColor(leftFrame,leftGray, CV_BGR2GRAY);
        if(!bothFramesArrived){
            leftFrameArrived.release();

        }
        qDebug() << "New rightFrame";

        //qDebug() << "New leftFrame";
    }else if(frameSide == "Right Camera"){
        //framesArrived.aquire(1);
        rightFrame = frame;
        cvtColor(rightFrame,rightGray, CV_BGR2GRAY);
        if(!bothFramesArrived){
            rightFrameArrived.release();
        }
        qDebug() << "New rightFrame";
    }
    */
}

/*
// Callback functions for minDisparity and numberOfDisparities trackbars
void on_minDisp(int min_disp, void * _disp_obj)
{
    disparity * disp_obj = (disparity *) _disp_obj;
    disp_obj -> set_minDisp(min_disp - 30);
}

void on_numDisp(int num_disp, void * _disp_obj)
{
    disparity * disp_obj = (disparity *) _disp_obj;
    num_disp = (num_disp / 16) * 16;
    setTrackbarPos("numDisparity", "Disparity", num_disp);
    disp_obj -> set_numDisp(num_disp);
}
*/

void DisparityCalcThread::beginDisparityCalcThread(QString fileName, Size image_size)
{
    fileName.append("/samples/stereo/stereo_calib.xml");
    qDebug() << "path to calibFile: "+fileName;

    // Read pixel maps from XML file
    FileStorage fs(fileName.toLocal8Bit().constData(), FileStorage::READ);
    qDebug() << "Calibration file procured";
    fs["map_l1"] >> map_l1;
    fs["map_l2"] >> map_l2;
    fs["map_r1"] >> map_r1;
    fs["map_r2"] >> map_r2;
    if(map_l1.empty() || map_l2.empty() || map_r1.empty() || map_r2.empty())
    {
        cout << "WARNING: Loading of mapping matrices not successful" << endl;
    }


    // Set SGBM parameters (from OpenCV stereo_match.cpp demo)

    /*
    stereo.preFilterCap = 63;
    stereo.SADWindowSize = 3;
    stereo.P1 = 8 * 3 * stereo.SADWindowSize * stereo.SADWindowSize;
    stereo.P2 = 32 * 3 * stereo.SADWindowSize * stereo.SADWindowSize;
    stereo.uniquenessRatio = 10;
    stereo.speckleWindowSize = 100;
    stereo.speckleRange = 32;
    stereo.disp12MaxDiff = 1;
    stereo.fullDP = true;
    */
   const string videoStreamAddress[] = { "http://admin:admin@192.168.0.100:80/video.mjpg", "http://admin:admin@192.168.0.102:80/video.mjpg" };
    videoLeft.beginVideoStream(videoStreamAddress[0],"left");
    videoRight.beginVideoStream(videoStreamAddress[1],"right");

    start();
}


void DisparityCalcThread::run()
{
    /*
    VideoCapture capr(1), capl(2);
    //reduce frame size
    capl.set(CV_CAP_PROP_FRAME_HEIGHT, image_size.height);
    capl.set(CV_CAP_PROP_FRAME_WIDTH, image_size.width);
    capr.set(CV_CAP_PROP_FRAME_HEIGHT, image_size.height);
    capr.set(CV_CAP_PROP_FRAME_WIDTH, image_size.width);
    */

    qDebug() << "Running DisparityCalcThread";

    //leftFrameArrived.acquire();
    //rightFrameArrived.acquire();

    qDebug() << "Both frames arrived";
    bothFramesArrived = true;

/*
    min_disp = 30;
    num_disp = ((image_size.width / 8) + 15) & -16;
    namedWindow("Disparity", CV_WINDOW_NORMAL);
    namedWindow("Left", CV_WINDOW_NORMAL);
    createTrackbar("minDisparity + 30", "Disparity", &min_disp, 60, on_minDisp, (void *)this);
    createTrackbar("numDisparity", "Disparity", &num_disp, 150, on_numDisp, (void *)this);
    on_minDisp(min_disp, this);
    on_numDisp(num_disp, this);
*/
    disp8U = Mat(leftGray.rows,leftGray.cols,CV_8UC1);
    disp16S = Mat(leftGray.rows,leftGray.cols,CV_16S);

    int min_disp = 6;
    int numDisparities = 16*20; //Must be divisible by 16.
    int SADWindowSize = 23; // Must be odd.

    //Ptr<StereoBM> sbm = StereoBM::create(numDisparities, SADWindowSize);
    //Ptr<StereoSGBM> sbm = StereoSGBM::create(min_disp,numDisparities, SADWindowSize);

    //Ptr<cuda::StereoBM> sbm = cuda::createStereoBM(numDisparities, SADWindowSize);

    namedWindow("Disparity Map");

    int numCudaDevices = cuda::getCudaEnabledDeviceCount();
    string s = to_string(numCudaDevices);
    cout << "Cuda enambled devices: "+s << endl;

    while(char(waitKey(1)) != 'q') {
        leftFrame = videoLeft.getCameraFeed();
        rightFrame = videoRight.getCameraFeed();


        if(leftFrame.empty()||rightFrame.empty()){
            //cout << "Waiting for video stream. Stand by." << endl;
            continue;
        }
        cvtColor(leftFrame,leftGray, CV_BGR2GRAY);
        cvtColor(rightFrame,rightGray, CV_BGR2GRAY);

        remap(leftGray, framel_rect, map_l1, map_l2, INTER_LINEAR);
        remap(rightGray, framer_rect, map_r1, map_r2, INTER_LINEAR);

        //sbm->compute(framel_rect,framer_rect, disp16S);
        //sbm->compute(framel_rect,framer_rect,gpu_disp,disp16S);

        minMaxLoc(disp16S, &minVal, &maxVal);

        disp16S.convertTo(disp8U, CV_8UC1, 255/(maxVal - minVal));

        //imshow("Left", leftGray);
        //imshow("Right", rightGray);

        if(!disp8U.empty())
            imshow("Disparity Map", disp8U);

        /*
        // Calculate disparity
        Mat disp, disp_show;
        stereo(framel_rect, framer_rect, disp);
        // Convert disparity to a form easy for visualization
        disp.convertTo(disp_show, CV_8U, 255/(stereo.numberOfDisparities * 16.));
        imshow("Disparity", disp_show);
        imshow("Left", framel);
        */
    }

    qDebug() << "DisparityCalcThread finished.";
}
