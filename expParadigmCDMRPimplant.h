#ifndef EXPPARADIGM_CDMRP_H
#define EXPPARADIGM_CDMRP_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include "FPGAControl.h"
#include "AdeptArmAPI.h"
#include <process.h>
#include "motorControl.h"

class expParadigmCDMRPimplant
{
    motorControl * motorObj;
    double perturbationAngle;
    HANDLE hIOMutex;
    bool robotPerturbationLive;
    int numberOfPerturbations;
    PPoint defaultPoint, newPoint;
    AdeptArmAPI adeptRobot;
    int perturbAdept();
    static void AdeptPerturbationsLoop(void* a);
    int rep[100];
    int numTrials;
    double amp[100];
    double freq[100];
public:
    int setPerturbationAngle(double angle);
    int setAdeptDefaultPosition(double * position);
    int startAdeptPerturbations(int numberOfPerturbations);
    expParadigmCDMRPimplant(motorControl *);
    void readData(void);
    int startParadigm(motorControl *motorObj);
    ~expParadigmCDMRPimplant(void);
};
#endif