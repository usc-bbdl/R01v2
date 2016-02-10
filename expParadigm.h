#ifndef EXPPARADIGM_H
#define EXPPARADIGM_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <analogClient.h>
#include <ctime>
#include <dataLogger.h>
#include "FPGAControl.h"
#include <servoControl.h>
class expParadigm
{
    analogClient *pClient;
    servoControl servo;
    float32 gammaDyn1[100],gammaSta1[100],gammaDyn2[100],gammaSta2[100];
    int rep[100];
    int numTrials;
    int trialLength[100];
    int initPos[100];
    int finalPos[100];
    int rampVelocity[100];
    //UdpClient client;
    dataLogger log;
public:
    expParadigm(double,double,analogClient*);
    ~expParadigm(void);
    int startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController);
    bool isRunning();
    int currentTrialNum,  currentRepNum;
    
};
#endif


