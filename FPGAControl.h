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
    int updateGamma(void);
    int updateSpindleParameters(void);
    int readMuscleFPGASpikeCount(void);
    int readMuscleFPGARaster_MN_1(void);
    int readMuscleFPGARaster_MN_2(void);
    int readMuscleFPGARaster_MN_3(void);
    int readMuscleFPGARaster_MN_4(void);
    int readMuscleFPGARaster_MN_5(void);
    int readMuscleFPGARaster_MN_6(void);
public:
    SomeFpga *muscleFPGA, *spindleFPGA;
    char updateGammaFlag,updateParametersFlag;
    FPGAControl(int, motorControl *);
    ~FPGAControl();
    
    
    float32 gammaDynamic, gammaStatic;
    float32 spindleIaGain, spindleIIGain, spindleIaOffset, spindleIIOffset, spindleIaSynapseGain, spindleIISynapseGain;
    void * threadRoutine(void *);
    float muscleLength, muscleVel , muscleForce, muscleForceFPGA , muscleEMG, spindleII , spindleIa ;
    int muscleSpikeCount, raster_MN_1,raster_MN_2,raster_MN_3,raster_MN_4,raster_MN_5,raster_MN_6;
};

#endif
