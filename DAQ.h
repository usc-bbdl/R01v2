#pragma once
#pragma once
#ifndef DAQ_H
#define DAQ_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>


#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
class Task
{
    TaskHandle task;
public:
    Task();
    ~Task();
};

class DAQ
{
public:
    DAQ(void);
    ~DAQ(void);
};

#endif