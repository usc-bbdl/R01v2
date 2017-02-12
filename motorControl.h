#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <NIDAQmx.h>
#include <utilities.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <ctime>
#include "motorData.h"

class TimeData
{
      LARGE_INTEGER initialTick, currentTick, frequency;
public:

    double actualTime;
    TimeData(void);	
    ~TimeData(void);
    int resetTimer();
    double getCurrentTime(void);
};

class motorControl
{
    TaskHandle  motorTaskHandle, motorEnableHandle,loadCelltaskHandle;
    TaskHandle  encodertaskHandle[2];
    TimeData timeData;
    HANDLE hIOMutex;
    bool live;
    bool isEnable, isWindUp, isControlling;
    //data file header
    char header[200];
    

    void controlLoop(void);
    static void motorControlLoop(void*);
public:
    motorData *mData;


    motorControl(double,double);
    ~motorControl(void);
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorControllerStart();
    int motorControllerEnd();
    void dummy();
};

#endif
/*

    */