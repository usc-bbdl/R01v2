#ifndef PARADIGM_MUSCLE_LENGTH_H
#define PARADIGM_MUSCLE_LENGTH_H
//#include <stdio.h>
//#include <windows.h>
//#include <iostream>
//#include <utilities.h>
//#include <ctime>
//#include <dataLogger.h>
//#include "FPGAControl.h"
//#include <servoControl.h>
class expParadigmMuscleLengthCalibration
{
    int initPos,finalPos;
    double gain[2], bias[2];
    const rampVelocity = 5, holdPeriod = 1000;
public:
    expParadigmMuscleLengthCalibration();
    ~expParadigmMuscleLengthCalibration(void);
    int startParadigm(servoControl *servo, motorControl *realTimeController);
    bool isRunning();
};
#endif


