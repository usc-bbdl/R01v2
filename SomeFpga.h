#ifndef SOMEFPGA_H
#define SOMEFPGA_H
#include <iostream>
#include "utilities.h"
//#include <pthread.h>
#include <malloc.h>
#include "okFrontPanelDLL.h"

class SomeFpga 
{
    public:
        SomeFpga(int , int , std::string);
        ~SomeFpga();
        int SendPara(int bitVal, int trigEvent);
        int SendButton(bool bitVal, int buttonEvt);
        int ReadFpga(BYTE, char *, float *);
        int ReadFpga(BYTE, char *,int *);
        int ReadFpga(BYTE, char *,uInt32 *);
        int ReadFpgaPipes(BYTE, char *, uInt32 *);
        //int ReadFpga(int getAddr);
        okCFrontPanel *xem;
        int WriteFpgaLceVel(int32 , int32 , int32 , int32 , int32);    
        int ReadFpgaPipes(BYTE, char *, float *);
        int WriteFpgaCortexDrive(int32, int32);
    private:
        int NUM_NEURON;
        int SAMPLING_RATE;
        char serX[50];
};

#endif