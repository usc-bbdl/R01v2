#ifndef DATA_ONE_SAMPLE_H
#define DATA_ONE_SAMPLE_H

//#include <NIDAQmx.h>
#include "DAQ.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "utilities.h"

class dataOneSample
{
public:
    dataOneSample();
    ~dataOneSample(void);
    double loadCell1, loadCell2;
};
#endif
