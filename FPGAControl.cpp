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
    //SomeFpga    *spindleFPGA;
    //SomeFpga    *muscleFPGA;
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
    while (live)
    {
        update();
    }
}
FPGAControl::~FPGAControl() {
    //delete all dynamic memory here
    delete this->spindleFPGA;
    delete this->muscleFPGA;
}

int FPGAControl::update() { //This is the function called in the thread


    muscleLength = pMotorControl->muscleLength[muscleIndex];
    muscleVel = pMotorControl->muscleVel[muscleIndex];
    writeSpindleLengthVel();
    writeMuscleFPGALengthVel();
    writeMuscleFPGALengthVel();
    //readSpindleIaFPGA();
    //readSpindleIIFPGA();
    readMuscleFPGAForce();
    //readEMG();
    pMotorControl->motorRef[muscleIndex] = muscleForce;
    switch (this->muscleIndex) {
    case 0:
        printf(" Bicep Length is: %+6.2f, muscle force is: %+6.2f, muscle Vel is: %6.2f \r",muscleLength, muscleForce,muscleVel);
        break;
    case 1:
        printf("Tricep Length is: %+6.2f, muscle force is: %+6.2f, muscle Vel is: %6.2f \r",muscleLength, muscleForce,muscleVel);
        break;
    }
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
    spindleFPGA->SendPara(bitValGammaSta, DATA_EVT_GAMMA_STA);
    return 0;
}

int FPGAControl::writeMuscleFPGALengthVel()
{
    float gMusDamp;
    //if(gAlterDamping && (gMusDamp>0.03f)) {
        gMusDamp = 0.0f;
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
    muscleFPGA->ReadFpga(0x32, "float32", &muscleForce);
    muscleForce = muscleForce *0.01;
    return 0;
}

int FPGAControl::readEMG()
{
    muscleFPGA->ReadFpga(0x20, "float32", &muscleEMG);
    return 0;
}