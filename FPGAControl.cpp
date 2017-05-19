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

    realTimeController->setSpindleIa(streamSensoryRead.firingRateIa, muscleIndex);
    realTimeController->setSpindleII(streamSensoryRead.firingRateII, muscleIndex);

    realTimeController->setMuscleReferenceForceScaling(streamMotorRead.muscleForce, muscleIndex);
    realTimeController->setMuscleEMG(streamMotorRead.muscleEMG, muscleIndex);
    realTimeController->setMuscleSpikeCount(streamMotorRead.spikeCount, muscleIndex);
    realTimeController->setRaster1(streamMotorRead.rasterMN1, muscleIndex);
    realTimeController->setRaster2(streamMotorRead.rasterMN2, muscleIndex);
    realTimeController->setRaster3(streamMotorRead.rasterMN3, muscleIndex);
    realTimeController->setRaster4(streamMotorRead.rasterMN4, muscleIndex);
    realTimeController->setRaster5(streamMotorRead.rasterMN5, muscleIndex);
    realTimeController->setRaster6(streamMotorRead.rasterMN6, muscleIndex);
}
int FPGAControl::writeRTC2FPGA()
{

    float muscleLength,muscleVelocity;
    realTimeController->getMuscleLength(&muscleLength,muscleIndex);
    realTimeController->getMuscleVelocity(&muscleVelocity,muscleIndex);
    streamSensoryWrite.length = muscleLength;
    streamSensoryWrite.velocity = muscleVelocity;
    //For now set gammas and gains to default values
    //Needs to be updated once we have muscle data class
    streamSensoryWrite.gammaDynamic = 0;
    streamSensoryWrite.gammaStatic = 0;
    streamSensoryWrite.IaGain = 0;
    streamSensoryWrite.IaOffset = 0;
    streamSensoryWrite.IIGain = 0;
    streamSensoryWrite.IIOffset = 0;
    
    streamMotorWrite.length = muscleLength;
    streamMotorWrite.velocity = muscleVelocity;
    //For now set gains to default values
    //Needs to be updated once we have muscle data class
    streamMotorWrite.synapseIaGain = 0;
    streamMotorWrite.synapseIIGain = 0;
    streamMotorWrite.synapseCortexGain = 0;
    
}
