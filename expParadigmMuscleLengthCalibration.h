#ifndef PARADIGM_MUSCLE_LENGTH_H
#define PARADIGM_MUSCLE_LENGTH_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "utilities.h"
//#include <ctime>
//#include <dataLogger.h>
//#include "FPGAControl.h"
#include "servoControl.h"
#include "motorControl.h"
class expParadigmMuscleLengthCalibration
{
    servoControl *servo;
    int initPos,finalPos;
    int rampVelocity, holdPeriod;
public:
    expParadigmMuscleLengthCalibration(servoControl *);
    ~expParadigmMuscleLengthCalibration(void);
    int expParadigmMuscleLengthCalibration::setInitFinalPos(int initPos, int finalPos);
    int startParadigm(motorControl *);
    bool isRunning();
};
#endif


