#include "FPGAControl.h"
#include "SomeFpga.h"
//#include <pthread.h>
#include <malloc.h>
#include "utilities.h"
#include "NIDAQmx.h"
#include "motorControl.h"
#include <windows.h>
#include <process.h>

//const char FPGAControl::spindleSerial[2][11] = {"113700021E", "11160001CG"};
//const char FPGAControl::muscleSerial[2][11]  = {"0000000542", "1137000222"};
bool killThread = 0;
int muscleIndex = 0;


FPGAControl::FPGAControl(int param, motorControl *param2)
{
    updateGammaFlag = '0';
    //SomeFpga    *spindleFPGA;
    //SomeFpga    *muscleFPGA;
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
    pMotorControl = param2;
    switch (param) {
    case 0:
        spindleFPGA = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "113700021E");
        muscleFPGA  = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "0000000542");
        break;
    case 1:
        spindleFPGA = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "11160001CG");
        muscleFPGA  = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "1137000222");
        break;
    default:
        std::cout<<"\nFATAL: Logical error in FPGAControl Params\n";
        std::abort();
    }
    initializeParameters();
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
    writeSpindleLengthVel(); //neck
    //writeMuscleFPGALengthVel(); //neck
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

int FPGAControl::updateGamma() {
    int32 bitValGammaDyn, bitValGammaSta;
    ReInterpret((float32)(gammaDynamic), &bitValGammaDyn);
    ReInterpret((float32)(gammaStatic), &bitValGammaSta);
    spindleFPGA->SendPara(bitValGammaDyn, DATA_EVT_GAMMA_DYN);
    Sleep(100);
    spindleFPGA->SendPara(bitValGammaSta, DATA_EVT_GAMMA_STA);
    Sleep(100);
    pMotorControl->gammaStatic = (int)(gammaStatic);
    pMotorControl->gammaDynamic = (int)(gammaDynamic);
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

int FPGAControl::readMuscleFPGAForce()
{
    muscleFPGA->ReadFpga(0x32, "float32", &muscleForceFPGA);
    //muscleForceFPGA = 0;
    float tCtrl = ((muscleForceFPGA) * GGAIN) +  TBIAS;
    muscleForce = (float)((tCtrl >= 0.0) ? tCtrl : 0.0f);
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