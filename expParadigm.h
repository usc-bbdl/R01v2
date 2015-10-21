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
    float32 gammaDyn[100];
    float32 gammaSta[100];
    float32 tgammaDyn[100];
    float32 tgammaSta[100];
    int rep[100];
    int numTrials;
    int trialLength[100];
    //UdpClient client;
    dataLogger log;
public:
    expParadigm(double,double,analogClient*);
    ~expParadigm(void);
    int startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA);
    bool isRunning();
    int currentTrialNum,  currentRepNum;
    
};
#endif


