#ifndef EXPPARADIGM_MANUAL_H
#define EXPPARADIGM_MANUAL_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <ctime>
#include "FPGAControl.h"
class expParadigmManualPerturbation
{
    float32 gammaDyn1,gammaSta1,gammaDyn2,gammaSta2,cortexDrive1,cortexDrive2;
public:
    expParadigmManualPerturbation();
    ~expParadigmManualPerturbation(void);
    int startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController);    
    bool isRunning();
};
#endif



