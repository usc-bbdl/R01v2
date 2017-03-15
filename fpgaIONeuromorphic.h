#ifndef FPGANEUROMORPHIC_H
#define FPGANEUROMORPHIC_H
#include <iostream>
#include "utilities.h"
//#include <pthread.h>
#include <malloc.h>

class SomeFpga 
{
    public:
        fpgaIONeuromorphic(int , int);
        ~fpgaIONeuromorphic();
        int readForceStream(float *);
        int readForceBlock(float *);
        int readForceWire(float *);
    private:
    	fpgaIO myFPGA;
    	int muscleType, fpgaType;
        char serX[50];
        int forceAdressWire, forceAdressPipe;
};

#endif
