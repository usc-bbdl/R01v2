#ifndef DATA_ONE_SAMPLE_H
#define DATA_ONE_SAMPLE_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

class dataOneSample
{
public:
    dataOneSample();
    ~dataOneSample(void);
    double loadCell[MUSCLE_NUM];
    double JR3_V[NUM_JR3_CHANNELS];
    double JR3_F[NUM_JR3_CHANNELS];
};
#endif
