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
    initializeParameters();

    forceLengthCurve = 1;

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
FPGAControl::~FPGAControl() {
    //delete all dynamic memory here
    /*
    delete this->spindleFPGA;
    delete this->muscleFPGA;
    */
}

int FPGAControl::update() { //This is the function called in the thread
    
    realTimeController->getMuscleLength(&muscleLength, muscleIndex);
    realTimeController->getMuscleVelocity(&muscleVel, muscleIndex);
    if (updateGammaFlag == '1') {
        updateGamma();
        updateGammaFlag = '0';
    }
    if (updateCortexFlag == '1') {
        updateCortexDrive();
        updateCortexFlag = '0';
    }
    if (updateParametersFlag == '1') {
        updateSpindleParameters();
        updateParametersFlag = '0';
    }
    writeSpindleLengthVel();
    //writeMuscleFPGALengthVel();
    if (dataAcquisitionFlag[0]){
        readMuscleFPGAForce();
        realTimeController->setMuscleReferenceForceScaling((float64) muscleForce, muscleIndex);
    }
    if (dataAcquisitionFlag[1]){
        readEMG();
        //realTimeController->mData->muscleEMG[muscleIndex] = muscleEMG;
    }
    if (dataAcquisitionFlag[2]){
        readSpindleIaFPGA();
       // realTimeController->mData->spindleIa[muscleIndex] = spindleIa;
    }
    if (dataAcquisitionFlag[3]){
        readSpindleIIFPGA();
       // realTimeController->mData->spindleII[muscleIndex] = spindleII;
    }
    if (dataAcquisitionFlag[4]){
        readMuscleFPGASpikeCount();
        //realTimeController->mData->muscleSpikeCount[muscleIndex] = muscleSpikeCount;
    }
    if (dataAcquisitionFlag[5]){
        readMuscleFPGARaster_MN_1();
        //realTimeController->mData->raster_MN_1[muscleIndex] = raster_MN_1;
    }
    if (dataAcquisitionFlag[6]){
        readMuscleFPGARaster_MN_2();
        //realTimeController->mData->raster_MN_2[muscleIndex] = raster_MN_2;
    }
    if (dataAcquisitionFlag[7]){
        readMuscleFPGARaster_MN_3();
        //realTimeController->mData->raster_MN_3[muscleIndex] = raster_MN_3;
    }
    if (dataAcquisitionFlag[8]){
        readMuscleFPGARaster_MN_4();
        //realTimeController->mData->raster_MN_4[muscleIndex] = raster_MN_4;
    }
    if (dataAcquisitionFlag[9]){
        readMuscleFPGARaster_MN_5();
        //realTimeController->mData->raster_MN_5[muscleIndex] = raster_MN_5;
    }
    if (dataAcquisitionFlag[10]){
        readMuscleFPGARaster_MN_6();
        //realTimeController->mData->raster_MN_6[muscleIndex] = raster_MN_6;
    }
    if (dataAcquisitionFlag[11]){
        //cortexDrive = (int32)realTimeController->mData->cortexDrive[muscleIndex];
        //writeCortexCommand();
        updateCortexDrive();
        if (muscleIndex==0)
            int a33 = 0;
        if (muscleIndex==1)
            int a33 = 1;
    }

    Sleep(5);
    return 0;
}

void * FPGAControl::threadRoutine(void *ptr) {
    while(!killThread) {
        update();
    }
    return NULL;
}
int FPGAControl::writeSpindleLengthVel()
{
    int32 bitValLce;
    ReInterpret((float32)(muscleLength), &bitValLce);
    this->spindleFPGA->SendPara(bitValLce, DATA_EVT_LCEVEL);
    return 0;
}

int FPGAControl::readSpindleIaFPGA()
{
    spindleFPGA->ReadFpga(0x22, "float32", &spindleIa);
    return 0;
}

int FPGAControl::readSpindleIIFPGA()
{
    spindleFPGA->ReadFpga(0x24, "float32", &spindleII);        
    return 0;
}

int FPGAControl::updateCortexDrive()
{
    int32 bitValCortexDrive;
    ReInterpret((int32)(cortexDrive), &bitValCortexDrive);
    cortexFPGA->SendPara(bitValCortexDrive, DATA_EVT_CORTEX_DRIVE);
    //Sleep(100);
    //realTimeController->mData->cortexDrive[muscleIndex] = (int)(cortexDrive);
    return 0;
}


int FPGAControl::updateGamma() {
    int32 bitValGammaDyn, bitValGammaSta;
    ReInterpret((float32)(gammaDynamic), &bitValGammaDyn);
    ReInterpret((float32)(gammaStatic), &bitValGammaSta);
    spindleFPGA->SendPara(bitValGammaDyn, DATA_EVT_GAMMA_DYN);
    Sleep(100);
    spindleFPGA->SendPara(bitValGammaSta, DATA_EVT_GAMMA_STA);
    Sleep(100);
    if (muscleIndex == 0)
    {
        //realTimeController->mData->gammaStatic[0] = (int)(gammaStatic);
        //realTimeController->mData->gammaDynamic[0] = (int)(gammaDynamic);
    }
    if (muscleIndex == 1)
    {
        //realTimeController->mData->gammaStatic[1] = (int)(gammaStatic);
        //realTimeController->mData->gammaDynamic[1] = (int)(gammaDynamic);
    }
    return 0;
}

int FPGAControl::updateSpindleParameters() {
    int32 bitValSpindleIaGain, bitValSpindleIIGain,bitValSpindleIaOffset, bitValSpindleIIOffset, bitValSpindleIaSynapseGain, bitValSpindleIISynapseGain, bitValForceLength;
    ReInterpret((float32)(spindleIaGain), &bitValSpindleIaGain);
    ReInterpret((float32)(spindleIIGain), &bitValSpindleIIGain);
    ReInterpret((float32)(spindleIaOffset), &bitValSpindleIaOffset);
    ReInterpret((float32)(spindleIIOffset), &bitValSpindleIIOffset);
    ReInterpret((float32)(spindleIaSynapseGain), &bitValSpindleIaSynapseGain);
    ReInterpret((float32)(spindleIISynapseGain), &bitValSpindleIISynapseGain);
    ReInterpret((int32)(forceLengthCurve), &bitValForceLength);
    
    spindleFPGA->SendPara(bitValSpindleIaGain, DATA_EVT_SPINDLE_IA_GAIN);
    Sleep(100);
    spindleFPGA->SendPara(bitValSpindleIIGain, DATA_EVT_SPINDLE_II_GAIN);
    Sleep(100);
    spindleFPGA->SendPara(bitValSpindleIaOffset, DATA_EVT_SPINDLE_IA_OFFSET);
    Sleep(100);
    spindleFPGA->SendPara(bitValSpindleIIOffset, DATA_EVT_SPINDLE_II_OFFSET);
    Sleep(100);
    muscleFPGA->SendPara(bitValSpindleIaSynapseGain, DATA_EVT_SYN_IA_GAIN);
    Sleep(100);
    muscleFPGA->SendPara(bitValSpindleIISynapseGain, DATA_EVT_SYN_II_GAIN);
    Sleep(100);
    muscleFPGA->SendPara(bitValForceLength, DATA_EVT_S_WEIGHT);
    Sleep(100);
    return 0;
}


int FPGAControl::writeMuscleFPGALengthVel()
{
    float gMusDamp;
    //if(gAlterDamping && (gMusDamp>0.03f)) {
    gMusDamp = 200.0f;
    //}
    int32   bitM1VoluntaryBic = 0, bitM1DystoniaBic = 000;
    int32 bitValLce, bitValVel;
    ReInterpret((float32)(gMusDamp * muscleVel), &bitValVel);
    ReInterpret((float32)(muscleLength), &bitValLce);
    muscleFPGA->WriteFpgaLceVel(bitValLce, bitValVel, bitM1VoluntaryBic, bitM1DystoniaBic, DATA_EVT_LCEVEL);
    return 0;
}
int FPGAControl::writeCortexCommand()
{
    int32 bitValCortexDrive;
    ReInterpret((float32)(cortexDriveMixed), &bitValCortexDrive);
    cortexFPGA->WriteFpgaCortexDrive(bitValCortexDrive, DATA_EVT_CORTEX_MIXED_INPUT);
    return 0;
}

int FPGAControl::readMuscleFPGAForce()
{
    muscleFPGA->ReadFpga(0x32, "float32", &muscleForceFPGA);
    //muscleForceFPGA = 0;
    float tCtrl = muscleForceFPGA;
    muscleForce = (float)((tCtrl >= 0.0) ? tCtrl : 0.0f);
    return 0;
}

int FPGAControl::readMuscleFPGASpikeCount()
{
    int spikeCount;
    muscleFPGA->ReadFpga(0x30, "int32", &spikeCount);
    muscleSpikeCount = spikeCount;
    //muscleForceFPGA = 0;
    return 0;
}

int FPGAControl::readMuscleFPGARaster_MN_1()
{
    muscleFPGA->ReadFpga(0x22, "int32", &raster_MN_1);
    return 0;
}
int FPGAControl::readMuscleFPGARaster_MN_2()
{
    muscleFPGA->ReadFpga(0x22, "int32", &raster_MN_2);
    return 0;
}
int FPGAControl::readMuscleFPGARaster_MN_3()
{
    muscleFPGA->ReadFpga(0x26, "int32", &raster_MN_3);
    return 0;
}
int FPGAControl::readMuscleFPGARaster_MN_4()
{
    muscleFPGA->ReadFpga(0x28, "int32", &raster_MN_4);
    return 0;
}
int FPGAControl::readMuscleFPGARaster_MN_5()
{
    muscleFPGA->ReadFpga(0x2A, "int32", &raster_MN_5);
    return 0;
}
int FPGAControl::readMuscleFPGARaster_MN_6()
{
    muscleFPGA->ReadFpga(0x2C, "int32", &raster_MN_6);
    return 0;
}
int FPGAControl::readEMG()
{
    muscleFPGA->ReadFpga(0x20, "float32", &muscleEMG);
    return 0;
}


int FPGAControl::initializeParameters()
{
    //int32 bitValSpindleGain;
    //int32 value = 60;
    //ReInterpret(value, &bitValSpindleGain);
    //this->muscleFPGA->SendPara(bitValSpindleGain, DATA_EVT_SYN_IA_GAIN);
    //value = 60;
    //ReInterpret(value, &bitValSpindleGain);
    //this->muscleFPGA->SendPara(bitValSpindleGain, DATA_EVT_SYN_II_GAIN);
    return 0;
}