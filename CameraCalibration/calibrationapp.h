#ifndef CALIBRATIONAPP_H
#define CALIBRATIONAPP_H

#include <QtCore>
#include "stdio.h"

#include "videostreamthread.h"
#include <globalparameters.h>


class CalibrationApp
{
public:
    CalibrationApp();
    void onCreate();


private:
    VideoStreamThread videoRight, videoLeft;

signals:

public slots:


};

#endif // CALIBRATIONAPP_H
