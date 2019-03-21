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

#define buffSize 1000

class expParadigmCDMRPimplant
{
    motorControl * motorObj;
    double defaultPos[6];
    double perturbationAngle;
    HANDLE hIOMutex;
    bool robotPerturbationLive;
    int numberOfPerturbations;
    PPoint defaultPoint, newPoint;
    AdeptArmAPI adeptRobot;
    int perturbAdept();    
    static void AdeptPerturbationsLoop(void* a);

    // SHELL ----------------------------------
    long thisTrial, Perts;
    double X, Y, Z, flexForce;
    PPoint dispPoint;
    int beginRobotShellThread(void);    
    static void perturbShellAdeptLoop(void* a);
    int perturbShellAdept();
    //-----------------------------------------
    
    long   numTrials, numPerts;
    double dispX[buffSize], dispY[buffSize], dispZ[buffSize];
    double flexorForce[buffSize];    
    
    int beginRobotPertThread (void);
public:
    void sweepAngleForce(double forceMin, double forceMax, double forceResolution, double  angleMin, double angleMax, double angleResolution, int numberOfPerturbations);
    
    // SHELL -------------------------
    void sweepShell3D();
    int setPerturbationShell(long trial, long perts, double x, double y, double z, double flexF);
    //--------------------------------

    int setPerturbationAngle(double angle);
    int setAdeptDefaultPosition();
    int startAdeptPerturbations(int numberOfPerturbations);
    expParadigmCDMRPimplant(motorControl *);
    void readData(void);
    //int startParadigm(motorControl *motorObj);
    ~expParadigmCDMRPimplant(void);
};
#endif