#ifndef FPGANEUROMORPHIC_H
#define FPGANEUROMORPHIC_H
#include <iostream>
#include "utilities.h"
#include <malloc.h>
#include <fpgaIO.h>

class fpgaIONeuromorphic 
{
    public:
        fpgaIONeuromorphic(int , int);
        ~fpgaIONeuromorphic();
        //Main FPGA communication functions
        int readFPGAStream(dataSensoryRead *);
        int readFPGAStream(dataMotorRead *);

        int writeFPGAStream(dataSensoryWrite *);
        int writeFPGAStream(dataMotorWrite *);

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
