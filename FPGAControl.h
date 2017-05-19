#ifndef FPGACONTROL_H
#define FPGACONTROL_H

#include "motorControl.h"
#include "fpgaIONeuromorphic.h"
class FPGAControl {

    fpgaIONeuromorphic spindleFPGA, cortexFPGA, motorFPGA;
    
    dataSensoryRead streamSensoryRead;
    dataSensoryWrite streamSensoryWrite;
    dataMotorRead streamMotorRead;
    dataMotorWrite streamMotorWrite;

    motorControl *realTimeController;
    bool killThread, connected2RTC;
    bool updateParametersFlag;
    int muscleIndex;
    HANDLE hIOMutex;
    bool live;

    static void FPGAControlLoop(void*);
    void controlLoop(void);
    int update(void);
    int writeFPGA2RTC(void);
    int writeRTC2FPGA(void);
    int updateFPGAParameters(void);
public:
    FPGAControl(int, motorControl *);
    ~FPGAControl();
    int updateFPGA();

};

#endif
