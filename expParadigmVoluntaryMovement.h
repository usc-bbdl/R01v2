#ifndef EXPPARADIGM_VOLUNTARY_MOVEMENT_H
#define EXPPARADIGM_VOLUNTARY_MOVEMENT_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <ctime>
#include "FPGAControl.h"
class expParadigmVoluntaryMovement
{
    float32 gammaDyn1,gammaSta1,gammaDyn2,gammaSta2;
    double *cortexAmp, *cortexFreq;
    bool vfTemp;

    
    unsigned long numTrials;
    const char *configFileName;
    
    double *FREQ, *CortexA, *GammaSA, *GammaSP, *GammaDA, *GammaDP;
    unsigned int *reps;

    double *vFREQ, *vCortexA, *vGammaSA, *vGammaSP, *vGammaDA, *vGammaDP;
    unsigned int *vreps;
    bool *vFlag;
    double *vtick, *vTempTick;

public:
    expParadigmVoluntaryMovement(motorControl *);
    ~expParadigmVoluntaryMovement(void);
    int startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController);    
    bool isRunning();
};
#endif


