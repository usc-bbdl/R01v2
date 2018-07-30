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
//const char FPGAControl::spindleSerial[2][11] = {"113700021E", "11160001CG"};
//const char FPGAControl::muscleSerial[2][11]  = {"0000000542", "1137000222"};
bool killThread = 0;
int muscleIndex = 0;


FPGAControl::FPGAControl(int param, motorControl *param2)
{
    //pcsa[0] = 0.56;
    pcsa[0] = 1.5;
    //pcsa[1] = 1.77;
    pcsa[1] = 1.5;
    theta[0] = 6 * 3.1416 / 180;
    theta[1] = 7 * 3.1416 / 180;
    updateGammaFlag = '0';
    updateParametersFlag = '0';
    //SomeFpga    *spindleFPGA;
    //SomeFpga    *muscleFPGA;
    muscleSpikeCount = 0;
    muscleForceFPGA = 0;
    muscleLength = 0;
    muscleVel = 0;
    muscleForce = 0;
    muscleEMG = 0;
    spindleII = 0;
    spindleIa = 0;
    this->muscleIndex = param;
    gammaDynamic = 0;
    gammaStatic = 0;
    spindleIaGain = spindleIIGain = spindleIaOffset = spindleIIOffset = spindleIaSynapseGain = spindleIISynapseGain =0;
    pMotorControl = param2;
    switch (param) {
    case 0:
        printf("BICEP : Spindle FPGA: ");
        spindleFPGA = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "113700021E"); // "113700021E or 000000053X
        printf("BICEP :  Muscle FPGA: ");
        muscleFPGA  = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "0000000542"); // "0000000542 or 000000054K
        printf("BICEP :  Cortex FPGA: ");
        cortexFPGA  = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "0000000547"); // "0000000547 or 0000000550
        break;
    case 1:
        printf("TRICEP: Spindle FPGA: ");
        spindleFPGA = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "11160001CG"); // 11160001CG or 11160001CJ
        printf("TRICEP:  Muscle FPGA: ");
        muscleFPGA  = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "1137000222"); // 000000054B or 12320003RN
        printf("TRICEP:  Cortex FPGA: ");
        cortexFPGA  = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "000000054B"); // 1137000222 or 12430003T2
        break;
    default:
        std::cout<<"\nFATAL: Logical error in FPGAControl Params\n";
        std::abort();
    }
    initializeParameters();

    gammaDynamic = 0;
    gammaStatic = 0;
    updateGamma();
    Sleep(500);
    updateGamma();
    Sleep(500);
    updateGamma();
    Sleep(500);
    cortexDrive = 0;
    forceLengthCurve = 1;
    updateCortexDrive();
    Sleep(500);
    printf("\nMuscle %d gamma update\n\n",muscleIndex);
    //pthread_create(&(this->thread), 0, &FPGAControl::threadRoutine, NULL);
    live = TRUE;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(FPGAControl::FPGAControlLoop,0,this);
}
void FPGAControl::FPGAControlLoop(void* a)
{
	((FPGAControl*)a)->controlLoop();
}
void FPGAControl::controlLoop(void){
    //gammaDynamic = 5;
    //gammaStatic = 5;
    //updateGamma();
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
    
    muscleLength = (float)pMotorControl->muscleLength[muscleIndex];
    muscleVel = (float)pMotorControl->muscleVel[muscleIndex];
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
        pMotorControl->motorRef[muscleIndex] = ((float64)muscleForce);
    }
    if (dataAcquisitionFlag[1]){
        readEMG();
        pMotorControl->muscleEMG[muscleIndex] = muscleEMG;
    }
    if (dataAcquisitionFlag[2]){
        readSpindleIaFPGA();
        pMotorControl->spindleIa[muscleIndex] = spindleIa;
    }
    if (dataAcquisitionFlag[3]){
        readSpindleIIFPGA();
        pMotorControl->spindleII[muscleIndex] = spindleII;
    }
    if (dataAcquisitionFlag[4]){
        readMuscleFPGASpikeCount();
        pMotorControl->muscleSpikeCount[muscleIndex] = muscleSpikeCount;
    }
    if (dataAcquisitionFlag[5]){
        readMuscleFPGARaster_MN_1();
        pMotorControl->raster_MN_1[muscleIndex] = raster_MN_1;
    }
    if (dataAcquisitionFlag[6]){
        readMuscleFPGARaster_MN_2();
        pMotorControl->raster_MN_2[muscleIndex] = raster_MN_2;
    }
    if (dataAcquisitionFlag[7]){
        readMuscleFPGARaster_MN_3();
        pMotorControl->raster_MN_3[muscleIndex] = raster_MN_3;
    }
    if (dataAcquisitionFlag[8]){
        readMuscleFPGARaster_MN_4();
        pMotorControl->raster_MN_4[muscleIndex] = raster_MN_4;
    }
    if (dataAcquisitionFlag[9]){
        readMuscleFPGARaster_MN_5();
        pMotorControl->raster_MN_5[muscleIndex] = raster_MN_5;
    }
    if (dataAcquisitionFlag[10]){
        readMuscleFPGARaster_MN_6();
        pMotorControl->raster_MN_6[muscleIndex] = raster_MN_6;
    }
    if (dataAcquisitionFlag[11]){
        cortexDrive =    (int32) pMotorControl->cortexDrive[muscleIndex];
        gammaStatic =  (float32) pMotorControl->gammaS[muscleIndex];
        gammaDynamic = (float32) pMotorControl->gammaD[muscleIndex];
        updateGamma();
        //writeCortexCommand();
        updateCortexDrive();
        if (muscleIndex==0)
            int a33 = 0;
        if (muscleIndex==1)
            int a33 = 1;
    }

    switch (this->muscleIndex) {
    case 0:
        //printf(" Bicep Length is: %+6.2f, muscle force is: %+6.2f, muscle Vel is: %6.2f \r",muscleLength, muscleForce,muscleVel);
        break;
    case 1:
        //printf("Tricep Length is: %+6.2f, muscle force is: %+6.2f, muscle Vel is: %6.2f \r",muscleLength, muscleForce,muscleVel);
        break;
    }
    Sleep(5);
    return 0;
}

void * FPGAControl::threadRoutine(void *ptr) {
    while(!killThread) {
        update();
    }
    //pthread_exit(0);
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
    pMotorControl->cortexDrive[muscleIndex] = (int)(cortexDrive);
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
        pMotorControl->gammaStatic1 = (int)(gammaStatic);
        pMotorControl->gammaDynamic1 = (int)(gammaDynamic);
    }
    if (muscleIndex == 1)
    {
        pMotorControl->gammaStatic2 = (int)(gammaStatic);
        pMotorControl->gammaDynamic2 = (int)(gammaDynamic);
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
    float tCtrl = ((muscleForceFPGA) * GGAIN);
    muscleForce = (float)((tCtrl >= 0.0) ? tCtrl : 0.0f);
    muscleForce = muscleForce*pcsa[muscleIndex]*cos(theta[muscleIndex])*22.54 + TBIAS;
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