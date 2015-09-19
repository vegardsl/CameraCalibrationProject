

#include "videostreamthread.h"
#include "globalparameters.h"

VideoStreamThread::VideoStreamThread(QObject *parent) : QThread(parent)
{

}



void VideoStreamThread::beginVideoStream(string address, string newWindowName){
   videoStreamAddress = address;
   windowName = newWindowName;
    start();
}

Mat VideoStreamThread::getCameraFeed(){
    //if(!exportedFeed.empty())
        return cameraFeed;
    //else return cameraFeed;
}

void VideoStreamThread::setRun(bool _continueRunning)
{
    continueRunning = _continueRunning;
}

void VideoStreamThread::run()
{

    qDebug() << "Running";

    VideoCapture capture;
    cout << "Opening video stream." << endl;

    capture.open(videoStreamAddress);

    if (!capture.isOpened())
    {
        std::cout << "Error when opening video stream: " << videoStreamAddress << std::endl;
        std::cout << "Press any key to quit. " << videoStreamAddress << std::endl;
        getchar();
        //return -1;
    }

    cout << "Video stream successfully opened. Please wait a few seconds... " << std::endl;

    int iterationCounter = 0;

    while (continueRunning){


        //store image to matrix
        //capture.grab();
        //capture.retrieve(cameraFeed);
        capture.read(cameraFeed);

        iterationCounter++;
        if(iterationCounter%30 == 0){
            //std::cout << "30 iterations performed on "+windowName << std::endl;
            //fastNlMeansDenoisingColored(cameraFeed, exportedFeed, 3,3,7,21);
            emit frameReady(cameraFeed, windowName);

        }

        //cout << "Time elapsed: " << t * 1000 / getTickFrequency() << std::endl;

        //imshow(windowName, cameraFeed);
        waitKey(30);
    }
    capture.release();
    /* VideoStreamThread is finished. */
}
