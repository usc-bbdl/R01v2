#ifndef FPGACONTROL_H
#define FPGACONTROL_H

//#include <pthread.h>
#include "motorControl.h"
#include "SomeFpga.h"

class FPGAControl {

    //static const char * spindleSerial[2][11];
    //static const char muscleSerial[2][11];
    motorControl *pMotorControl;
    //pthread_t thread;
    bool killThread;
    int muscleIndex;
    HANDLE hIOMutex;
    bool live;
    static void FPGAControlLoop(void*);
    void controlLoop(void);

     
    int update(void);
    int writeSpindleLengthVel(void);
    int readSpindleIaFPGA(void);
    int readSpindleIIFPGA(void);
    int writeMuscleFPGALengthVel(void);
    int readMuscleFPGAForce(void);
    int readEMG(void);
    int initializeParameters();

public:
    SomeFpga *muscleFPGA, *spindleFPGA;
    FPGAControl(int, motorControl *);
    ~FPGAControl();
    int updateGamma(void);
    float32 gammaDynamic, gammaStatic;
    void * threadRoutine(void *);
    float muscleLength, muscleVel , muscleForce, muscleForceFPGA , muscleEMG, spindleII , spindleIa ;
};

#endif
