#include "singlecamcalibrator.h"

SingleCamCalibrator::SingleCamCalibrator()
{

}


void SingleCamCalibrator::generateCalibrationSamples(QDir sampleFilePath, QString selectedCamera)
{
    qDebug() << "Selected camera is: "+selectedCamera;
    const string videoStreamAddress[] = { "http://admin:admin@192.168.0.100:80/video.mjpg", "http://admin:admin@192.168.0.102:80/video.mjpg" };
    if(selectedCamera.compare("left")){
        mVideoStreamThread.beginVideoStream(videoStreamAddress[0],selectedCamera.toLocal8Bit().constData()); // Hack from QString to std string
    }else if(selectedCamera.compare("right")){
        mVideoStreamThread.beginVideoStream(videoStreamAddress[1],selectedCamera.toLocal8Bit().constData()); // Hack from QString to std string
    }

    vector<Point3f> ob_p;
    for(int i = 0; i < height_internalCorners; i++) {
        for(int j = 0; j < width_internalCorners; j++) {
            ob_p.push_back(Point3f(j * squareSideLength, i * squareSideLength, 0.f));
        }
    }


    namedWindow("Chessboard corners");

    vector<Point2f> corners;
    bool chessBoardDetected = false;

    int numberOfSamples = 0;

    cout << "Press 'c' to capture ..." << endl;
    char choice = 'z';
    Mat im_show;
    int count = 0;
    while (numberOfSamples < 10 || choice != 'q'){
        cameraFeed = mVideoStreamThread.getCameraFeed();
        if(cameraFeed.empty()){
            //cout << "Waiting for video stream. Stand by." << endl;
            continue;
        }
        //imshow("Received CamFeed",cameraFeed);

        chessBoardDetected = findChessboardCorners(cameraFeed, Size(width_internalCorners, height_internalCorners), corners,
        CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        if(chessBoardDetected){
            im_show = cameraFeed.clone();
            drawChessboardCorners(im_show, Size(width_internalCorners, height_internalCorners), corners, chessBoardDetected);
            imshow("Chessboard corners", im_show);
            chessBoardDetected = false;
            if(choice == 'c'){
                //cvtColor(im_show, im_invert, CV_BGR2);
                //Mat im_invert = ~im_show;
                imshow("Chessboard corners", ~im_show);
                waitKey(200);

                vector<int> compression_params;
                compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
                compression_params.push_back(9);

                QString fileName = "/"+selectedCamera;
                fileName.append(QString::number(count));
                fileName.append(".png");

                QString pathString = sampleFilePath.path();
                pathString.append(fileName);

                Mat gray;
                cvtColor(cameraFeed, gray, CV_BGR2GRAY);

                imwrite(pathString.toLocal8Bit().constData(), gray,compression_params);

                count++;
            }

        }else{
            imshow("Chessboard corners", cameraFeed);
        }

        choice = char(waitKey(1));

    }
    mVideoStreamThread.setRun(false);
    while(!mVideoStreamThread.isFinished()){
        // wait for thread to finish.
    }
}




int SingleCamCalibrator::initSingleCamCalibrator(QString _selectedCamera,// Change to QString!
                                                  QDir _appDirPath,
                                                  double _squareSideLength,
                                                  int _width_internalCorners,
                                                  int _height_internalCorners)
{
    selectedCamera = _selectedCamera;
    squareSideLength = _squareSideLength;
    width_internalCorners = _width_internalCorners;
    height_internalCorners = _height_internalCorners;
    appDirPath = _appDirPath;
    qDebug() << "Selected camera is: "+selectedCamera;
    const string videoStreamAddress[] = { "http://admin:admin@192.168.0.100:80/video.mjpg", "http://admin:admin@192.168.0.102:80/video.mjpg" };

    // Read images
    QString path = appDirPath.absolutePath();
    if(selectedCamera.contains("left")){
        qDebug() << "Left compare";
        path.append("/samples/single/left/");
        sampleFilePath.setPath(path);

    }else if(selectedCamera.contains("right")){
        qDebug() << "Right compare";
        path.append("/samples/single/right/");
        sampleFilePath.setPath(path);

    }else{
        qDebug() << "SingleCamCalibrator::initSingleCamCalibrator(): Unvalid camera selection.";
        return FAILED;
    }

    qDebug() << sampleFilePath.absolutePath();

    if(sampleFilePath.count()<3){ // Testing if there are sample images for calibration.
      generateCalibrationSamples(sampleFilePath, selectedCamera);
    }
    if(sampleFilePath.count()<3){ // Testing if samples were really generated.
        return FAILED;
    }

    QString fileName;
    QDirIterator directory_iterator(sampleFilePath, QDirIterator::Subdirectories);
    Mat tempMat;


    while(directory_iterator.hasNext()){
        //qDebug() << directory_iterator.next();

        fileName = sampleFilePath.filePath(directory_iterator.next());
        string file = fileName.toUtf8();

        tempMat = imread(file);
        if(!tempMat.empty()&& tempMat.data)
        {
            images.push_back(imread(file.c_str()));
            cout << file << endl;
        }
    }

    /*
    for(directory_iterator i(sampleFilePath), end_iter; i != end_iter; i++) {
        string filename = sampleFilePath + i->path().filename().string();
        images.push_back(imread(filename));
    }
    */
    return SUCCESS;
}

void SingleCamCalibrator::calc_image_points(bool show) {
    // Calculate the object points in the object co-ordinate system (origin at top left corner)
    vector<Point3f> ob_p;
    for(int i = 0; i < height_internalCorners; i++) {
        for(int j = 0; j < width_internalCorners; j++) {
            ob_p.push_back(Point3f(j * squareSideLength, i * squareSideLength, 0.f));
        }
    }

    if(show) namedWindow("Chessboard corners");
    for(int i = 0; i < images.size(); i++) {
        Mat im = images[i];
        vector<Point2f> corners;
        //find corners in the chessboard image
        qDebug() << "Looking for chessboard corners";
        bool pattern_found = false;
        try
        {
            pattern_found = findChessboardCorners(im, Size(width_internalCorners, height_internalCorners), corners,
            CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        }
        catch(Exception& e)
        {
            const char* err_msg = e.what();
            cout << "'findChessboardCorners': Exception caught: " << err_msg << endl;
        }

        if(pattern_found) {
            qDebug() << "Found pattern!";

            object_points.push_back(ob_p);
            Mat gray;
            cvtColor(im, gray, CV_BGR2GRAY);
            //cornerSubPix(gray, im_p, Size(6, 9), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS +
            //CV_TERMCRIT_ITER, 30, 0.1));

            qDebug() << "Break!";
            try
            {
                image_points.push_back(corners);
            }
            catch(Exception& e)
            {
                const char* err_msg = e.what();
                cout << "'image_points.push_back(corners);': Exception caught: " << err_msg << endl;
            }

            if(show) {

                Mat im_show = im.clone();
                try
                {
                    drawChessboardCorners(im_show, Size(width_internalCorners, height_internalCorners), corners, pattern_found);
                    imshow("Chessboard corners", im_show);
                }
                catch(Exception& e)
                {
                    const char* err_msg = e.what();
                    cout << "'drawChessboardCorners': Exception caught: " << err_msg << endl;
                }

                //while(char(waitKey(1)) != ' ') {}
                waitKey();
            }
        }
        //if a valid pattern was not found, delete the entry from vector of images
        else images.erase(images.begin() + i);
    }
}
void SingleCamCalibrator::calibrate() {
    vector<Mat> rvecs, tvecs;
    float rms_error = calibrateCamera(object_points, image_points, images[0].size(), cameraMatrix,
    distCoeffs, rvecs, tvecs);
    cout << "RMS reprojection error " << rms_error << endl;
}

Mat SingleCamCalibrator::get_cameraMatrix() {
    return cameraMatrix;
}

Mat SingleCamCalibrator::get_distCoeffs() {
    return distCoeffs;
}

