#ifndef EXPPARADIGM_H
#define EXPPARADIGM_H
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
    servoControl servo;
    float32 gammaDyn1[100],gammaSta1[100],gammaDyn2[100],gammaSta2[100],cortexDrive1[100],cortexDrive2[100];
    int rep[100];
    int numTrials;
    int trialLength[100];
    int initPos[100];
    int finalPos[100];
    int rampVelocity[100];
    dataLogger log;
public:
    int currentTrialNum,  currentRepNum;
    expParadigmServoPerturbation(double,double);
    ~expParadigmServoPerturbation(void);
    int startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController);    
    bool isRunning();
};
#endif


