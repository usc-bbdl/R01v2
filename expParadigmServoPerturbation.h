#ifndef EXPPARADIGM_SERVO_H
#define EXPPARADIGM_SERVO_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <ctime>
#include <dataLogger.h>
#include "FPGAControl.h"
#include <servoControl.h>
class expParadigmServoPerturbation
{
    servoControl *servo;
    float32 gammaDyn1[1000],gammaSta1[1000],gammaDyn2[1000],gammaSta2[1000],cortexDrive1[1000],cortexDrive2[1000];
    int rep[1000];
    int numTrials;
    int trialLength[1000];
    int initPos[1000];
    int finalPos[1000];
    int rampVelocity[1000];
    dataLogger log;
public:
    int currentTrialNum,  currentRepNum;
    expParadigmServoPerturbation(double,double,servoControl *);
    ~expParadigmServoPerturbation(void);
    int startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController);    
    bool isRunning();
};
#endif


