#include "FPGAControl.h"
#include "SomeFpga.h"
//#include <pthread.h>
#include <malloc.h>
#include "utilities.h"
#include "NIDAQmx.h"
#include "motorControl.h"
#include <windows.h>
#include <process.h>
#include <math.h>
#include "fpgaIONeuromorphic.h"
bool killThread = 0;
int muscleIndex = 0;
FPGAControl::FPGAControl(int muscleIndex, motorControl *realTimeController)
{
    this->muscleIndex = muscleIndex;
    this->realTimeController = realTimeController;
    fpgaIONeuromorphic spindleFPGA(SPINDLE,muscleIndex);
    fpgaIONeuromorphic cortexFPGA(CORTEX,muscleIndex);
    fpgaIONeuromorphic motorFPGA(MOTOR,muscleIndex);
    connected2RTC = false;
    updateParametersFlag = true;
    live = TRUE;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(FPGAControl::FPGAControlLoop,0,this);
}
void FPGAControl::FPGAControlLoop(void* a)
{
	((FPGAControl*)a)->controlLoop();
}
void FPGAControl::controlLoop(void){
    while (live)
    {
        update();
    }
}
FPGAControl::~FPGAControl() 
{
}

int FPGAControl::update() { //This is the function called in the thread
    spindleFPGA.readFPGAStream(&streamSensoryRead);
    //cortexFPGA.readFPGAStream(&streamCortexRead);
    motorFPGA.readFPGAStream(&streamMotorRead);

    spindleFPGA.writeFPGAStream(streamSensoryWrite);
    //cortexFPGA.writeFPGAStream(streamCortexRead);
    motorFPGA.writeFPGAStream(streamMotorWrite);
    if (connected2RTC)
    {
        writeFPGA2RTC();
        writeRTC2FPGA();
    }
    if (updateParametersFlag) {
        updateFPGAParameters();
        updateParametersFlag = false;
    }
    
    return 1;
}
int FPGAControl::updateFPGA()
{
    updateParametersFlag = true;
}
int FPGAControl::writeFPGA2RTC()
{

    realTimeController->setSpindleIa(streamSensoryRead.firingRateIa);
    realTimeController->setSpindleII(streamSensoryRead.firingRateII);

    realTimeController->setMuscleReferenceForceScaling(streamMotorRead.muscleForce, muscleIndex);
    realTimeController->setMuscleEMG(streamMotorRead.muscleEMG, muscleIndex);
    realTimeController->setMuscleSpikeCount(streamMotorRead.spikeCount, muscleIndex);
    realTimeController->setRaster1(streamMotorRead.rasterMN1, muscleIndex);
    realTimeController->setRaster2(streamMotorRead.rasterMN1, muscleIndex);
    realTimeController->setRaster3(streamMotorRead.rasterMN1, muscleIndex);
    realTimeController->setRaster4(streamMotorRead.rasterMN1, muscleIndex);
    realTimeController->setRaster5(streamMotorRead.rasterMN1, muscleIndex);
    realTimeController->setRaster6(streamMotorRead.rasterMN1, muscleIndex);
}
int FPGAControl::writeRTC2FPGA()
{
    
}
