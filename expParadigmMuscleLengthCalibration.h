#ifndef PARADIGM_MUSCLE_LENGTH_H
#define PARADIGM_MUSCLE_LENGTH_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
//#include <ctime>
//#include <dataLogger.h>
//#include "FPGAControl.h"
#include <servoControl.h>
#include <motorControl.h>
class expParadigmMuscleLengthCalibration
{
    servoControl *servo;
    int initPos,finalPos;
    double gain[2], bias[2];
    int rampVelocity, holdPeriod;
public:
    expParadigmMuscleLengthCalibration(servoControl *);
    ~expParadigmMuscleLengthCalibration(void);
    int startParadigm(motorControl *);
    bool isRunning();
};
#endif


