#include "stereocameracalibration.h"

StereoCameraCalibration::StereoCameraCalibration(){

}

void StereoCameraCalibration::initStereoCameraCalibration(QDir _appDir, QString _l_path, QString _r_path, float _side_length, int _width, int _height)
{
    qDebug() << "StereoCameraCalibration has been called.";

    side_length = _side_length;
    width = _width;
    height = _height;
    l_path = _l_path;
    r_path = _r_path;
    appDir = _appDir;
/*
    // Read image samples
    for(directory_iterator i(l_path), end_iter; i != end_iter; i++)
    {
        string im_name = i->path().filename().string();
        string l_filename = l_path + im_name;
        im_name.replace(im_name.begin(), im_name.begin() + 4, string("right"));
        string r_filename = r_path + im_name;
        Mat lim = imread(l_filename), rim = imread(r_filename);

        if(!lim.empty() && !rim.empty())
        {
            l_images.push_back(lim);
            r_images.push_back(rim);
        }
    }
*/

   getStoredSamples("left");
   getStoredSamples("right");
}

void StereoCameraCalibration::getStoredSamples(QString side)
{
    QString filePath;

    QDir l_dir(l_path);
    QDir r_dir(r_path);
    if(l_dir.count()<3 || r_dir.count()<3){ // Testing if there are sample images for calibration.
      cout << "Samples lacking in either left or right folder." << endl;
      cout << "Gathering new samples." << endl;
      generateCalibrationStereoSamples(l_dir, r_dir);
    }

    if(side.contains("left"))
    {

        QDirIterator directory_iterator(l_dir, QDirIterator::Subdirectories);

        while(directory_iterator.hasNext()){
            //qDebug() << "Sample to be added to image list: "+directory_iterator.next();
            //QString imageFilePath = appPath;
            filePath = appDir.filePath(directory_iterator.next());
            //imageFilePath.append("/");
            //imageFilePath.append(fileName);

            qDebug() << "Sample to be added to left image list: "+filePath;

            string std_imageFilePath = filePath.toLocal8Bit().constData();
            cout << std_imageFilePath << endl;

            pushBackSample(std_imageFilePath, "left");
        }
    }
    else if (side.contains("right"))
    {

        QDirIterator directory_iterator(r_dir, QDirIterator::Subdirectories);

        while(directory_iterator.hasNext()){
            //qDebug() << "Sample to be added to image list: "+directory_iterator.next();
            //QString imageFilePath = appPath;
            filePath = appDir.filePath(directory_iterator.next());
            //imageFilePath.append("/");
            //imageFilePath.append(fileName);
            qDebug() << "Sample to be added to right image list: "+filePath;
            string std_imageFilePath = filePath.toLocal8Bit().constData();
            cout << std_imageFilePath << endl;

            pushBackSample(std_imageFilePath, "right");
        }
    }
    else
    {
        qDebug() << "No valid camera option.";
    }
}

void StereoCameraCalibration::generateCalibrationStereoSamples(QDir l_dir, QDir r_dir)
{

    QString l_path = l_dir.absolutePath();
    QString r_path = r_dir.absolutePath();

    const string videoStreamAddress[] = { "http://admin:admin@192.168.0.100:80/video.mjpg", "http://admin:admin@192.168.0.102:80/video.mjpg" };

    leftVideoStreamThread.beginVideoStream(videoStreamAddress[0],"left");
    rightVideoStreamThread.beginVideoStream(videoStreamAddress[1],"right");

    cout << "Press 'c' to capture ..." << endl;
    char choice = 'z';
    int sampleCount = 0;

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    while (sampleCount < 10 || choice != 'q'){

        leftCameraFeed = leftVideoStreamThread.getCameraFeed();
        rightCameraFeed = rightVideoStreamThread.getCameraFeed();

        if(leftCameraFeed.empty()||rightCameraFeed.empty()){
            //cout << "Waiting for video stream. Stand by." << endl;
            continue;
        }

        l_imshow = leftCameraFeed.clone();
        r_imshow = rightCameraFeed.clone();

        if(choice == 'c'){

            imshow("Left", ~l_imshow);
            imshow("Right", ~r_imshow);
            waitKey(200);


            QString l_path = l_dir.absolutePath();
            QString r_path = r_dir.absolutePath();
            QString l_fileName = "/left";
            QString r_fileName = "/right";
            l_fileName.append(QString::number(sampleCount));
            r_fileName.append(QString::number(sampleCount));
            l_fileName.append(".png");
            r_fileName.append(".png");

            l_path.append(l_fileName);
            r_path.append(r_fileName);

            cvtColor(leftCameraFeed, l_gray, CV_BGR2GRAY);
            cvtColor(rightCameraFeed, r_gray, CV_BGR2GRAY);

            imwrite(l_path.toLocal8Bit().constData(), l_gray,compression_params);
            imwrite(r_path.toLocal8Bit().constData(), r_gray,compression_params);

            sampleCount++;
        }
        else
        {
            imshow("Left", l_imshow);
            imshow("Right", r_imshow);
        }

        choice = char(waitKey(1));
    }

    leftVideoStreamThread.setRun(false);
    rightVideoStreamThread.setRun(false);
    while(!leftVideoStreamThread.isFinished()||rightVideoStreamThread.isFinished()){
        // wait for thread to finish.
    }
    qDebug() << "Both threads are finished.";
}

void StereoCameraCalibration::pushBackSample(string file, QString side)
{

    Mat tempMat;
    tempMat = imread(file);
    if(!tempMat.empty()&& tempMat.data)
    {
        if(side.contains("left"))
        {
            l_images.push_back(imread(file.c_str()));
            qDebug() << "Sample added to left image list.";
        }
        else if(side.contains("right"))
        {
            qDebug() << "Sample added to right image list.";
            r_images.push_back(imread(file.c_str()));
        }
        else
        {
            qDebug() << "No valid camera option.";
        }
        cout << file << endl;

    }
}

void StereoCameraCalibration::calc_image_points(bool show)
{

    // Calculate the object points in the object co-ordinate system (origin at top left corner)
    vector<Point3f> ob_p;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            ob_p.push_back(Point3f(j * side_length, i * side_length, 0.f));
        }
    }

    if(show) {
        namedWindow("Left Chessboard corners");
        namedWindow("Right Chessboard corners");
    }
    qDebug() << "Number of recorded samples: "+QString::number(l_images.size());
    for(int i = 0; i < l_images.size(); i++) {
        Mat lim = l_images[i], rim = r_images[i];
        vector<Point2f> l_im_p, r_im_p;
        bool l_pattern_found = findChessboardCorners(lim, Size(width, height), l_im_p,
        CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+ CALIB_CB_FAST_CHECK);
        bool r_pattern_found = findChessboardCorners(rim, Size(width, height), r_im_p,
        CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+ CALIB_CB_FAST_CHECK);

        if(l_pattern_found && r_pattern_found) {
            object_points.push_back(ob_p);
            Mat gray;
            cvtColor(lim, gray, CV_BGR2GRAY);
            cornerSubPix(gray, l_im_p, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS +
            CV_TERMCRIT_ITER, 30, 0.1));
            cvtColor(rim, gray, CV_BGR2GRAY);
            cornerSubPix(gray, r_im_p, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS +
            CV_TERMCRIT_ITER, 30, 0.1));
            l_image_points.push_back(l_im_p);
            r_image_points.push_back(r_im_p);

            if(show) {
                Mat im_show = lim.clone();
                drawChessboardCorners(im_show, Size(width, height), l_im_p, true);
                imshow("Left Chessboard corners", im_show);
                im_show = rim.clone();
                drawChessboardCorners(im_show, Size(width, height), r_im_p, true);
                imshow("Right Chessboard corners", im_show);

                while(char(waitKey(1)) != ' ') {}
            }
        }else {
            l_images.erase(l_images.begin() + i);
            r_images.erase(r_images.begin() + i);
        }
    }
}

bool StereoCameraCalibration::calibrate()
{
    cout << "Calibrating stereo vision." << endl;
    string filename = "samples/single/left/" + string("cam_calib.xml");
    FileStorage fs(filename, FileStorage::READ);
    fs["cameraMatrix"] >> l_cameraMatrix;
    fs["distCoeffs"] >> l_distCoeffs;
    fs.release();

    filename = "samples/single/right/" + string("cam_calib.xml");
    fs.open(filename, FileStorage::READ);
    fs["cameraMatrix"] >> r_cameraMatrix;
    fs["distCoeffs"] >> r_distCoeffs;
    fs.release();
    cout << "Checking input data." << endl;
    if(!l_cameraMatrix.empty()
            && !l_distCoeffs.empty()
            && !r_cameraMatrix.empty()
            && !r_distCoeffs.empty())
    {
        cout << "Input is valid." << endl;
        double rms = stereoCalibrate(object_points,
                                     l_image_points,
                                     r_image_points,
                                     l_cameraMatrix,
                                     l_distCoeffs,
                                     r_cameraMatrix,
                                     r_distCoeffs,
                                     l_images[0].size(),
                                     R, T, E, F
                                     );

        cout << "Calibrated stereo camera with a RMS error of " << rms << endl;
        filename = "samples/stereo/" + string("stereo_calib.xml");
        fs.open(filename, FileStorage::WRITE);
        fs << "l_cameraMatrix" << l_cameraMatrix;
        fs << "r_cameraMatrix" << r_cameraMatrix;
        fs << "l_distCoeffs" << l_distCoeffs;
        fs << "r_distCoeffs" << r_distCoeffs;
        fs << "R" << R;
        fs << "T" << T;
        fs << "E" << E;
        fs << "F" << F;
        cout << "Calibration parameters saved to " << filename << endl;
        return true;
    }
    else return false;
}
