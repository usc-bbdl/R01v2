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
        int readForceWire(float *);
    private:
        char serX[50];
};

#endif
