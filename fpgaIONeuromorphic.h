#ifndef FPGANEUROMORPHIC_H
#define FPGANEUROMORPHIC_H
#include <iostream>
#include "utilities.h"
#include <malloc.h>
#include <fpgaIO.h>
const int   SPINDLE = 0;
const int   MOTOR = 1;
const int   CORTEX = 2;

const int  BICEP = 0;
const int TRICEPS = 1;

struct dataSensoryRead{
    float32 firingRateIa;
    float32 firingRateII;
    float32 mixedInput;
    int32 populationNeuron;
};
struct dataSensoryWrite{
    float length;
    float velocity;
    float gammaDynamic;
    float gammaStatic;
    float IaGain;
    float IaOffset;
    float IIGain;
    float IIOffset;
};
struct dataMotorRead{
    float32 muscleForce;
    float32 muscleEMG;
    int spikeCount;
    int32 rasterMN1;
    int32 rasterMN2;
    int32 rasterMN3;
    int32 rasterMN4;
    int32 rasterMN5;
    int32 rasterMN6;
};
struct dataMotorWrite{
    float synapseIaGain;
    float synapseIIGain;
    float synapseCortexGain;
    float length;
    float velocity;
};

struct dataCortexRead{
    //TBD
};
struct dataCortexWrite{
    int32 cortexDrive;
    //TBD
};
class fpgaIONeuromorphic 
{
    public:
        fpgaIONeuromorphic(int fpgaType, int muscleType);
        ~fpgaIONeuromorphic();
        //Main FPGA communication functions
        int readFPGAStream(dataSensoryRead *);
        int readFPGAStream(dataMotorRead *);
        int readFPGAStream(dataCortexRead *);

        int writeFPGAStream(dataSensoryWrite);
        int writeFPGAStream(dataMotorWrite);
        int writeFPGAStream(dataCortexWrite);

        //Test functions to study synchrony between wire and stream and shall be deleted in future versions
        int readForceStream(float *);
        int readForceBlock(float *);
        int readForceWire(float *);
        int writeLengthVelocityWire(float *);

    private:
    	fpgaIO myFPGA;
    	int muscleType, fpgaType;
        char serX[50];
        int forceAdressWire,forceAdressPipe,forceAdressBlock, readMuscleSignals;
        float muscleForceWire, muscleForcePipe;
};

#endif
