#include <QCoreApplication>
#include "calibrationapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CalibrationApp calibrationApp;
    //calibrationApp.onCreate();

    return a.exec();
}

