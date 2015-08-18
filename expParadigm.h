#ifndef EXPPARADIGM_H
#define EXPPARADIGM_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <analogClient.h>
#include <ctime>
#include <dataLogger.h>

class expParadigm
{
    analogClient *pClient;
    double gammaDyn[100];
    double gammaSta[100];
    int rep[100];
    int numTrials;
    int trialLength[100];
    //UdpClient client;
    dataLogger log;
public:
    expParadigm(double,double,analogClient*);
    ~expParadigm(void);
    int startParadigm();
    bool isRunning();
    int currentTrialNum,  currentRepNum;
    
};
#endif


