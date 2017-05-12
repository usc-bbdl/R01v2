#include "utilities.h"
#include <stdio.h>
#include <conio.h>

//#include <FPGAControl.h>

//float GGAIN = 0.0125; //default is (0.9/1000) //0.4/2000 is safe
float GGAIN = 0.000016923;//0.005; //default is (0.9/1000) //0.4/2000 is safe
float TBIAS = 2;

int dataAcquisitionFlag[12] = {1,0,0,0,0,0,0,0,0,0,0,0}; //force(0), EMG(1), spindleIa(2), spindleII(3),spikeCount(4),raster1(5),raster2(6),raster3(7),raster4(8),raster5(9),raster6(10), real-time control cortex(11)


int ReInterpret(float32 in, int32 *out)
{
    memcpy(out, &in, sizeof(int32));
    return 0;
}

int ReInterpret(int32 in, int32 *out)
{
    memcpy(out, &in, sizeof(int32));
    return 0;
}

int ReInterpret(int in, float *out)
{
    memcpy(out, &in, sizeof(float));
    return 0;
}
