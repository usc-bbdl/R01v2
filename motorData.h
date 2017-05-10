#pragma once

#include <stdio.h>
#include <utilities.h>
#include "DAQ.h"


class motorData
{
public:
    /* these were private before */
    double I;
    bool resetMuscleLength;
    bool newPdgm_Flag;
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    int *muscleSpikeCount, *raster_MN_1, *raster_MN_2, *raster_MN_3, *raster_MN_4, *raster_MN_5, *raster_MN_6;
    int *gammaDynamic, *gammaStatic;
    int trialTrigger;
    double angle, velocity;
    /* end of private list */
    
    float64 *newPdgm_ref;
    /* pointer variables for use with DAQ class */
    float64 *loadCellData;
    double *loadCellOffset;
    float64 *motorRef;
    float64 *muscleLength;
    float64 *muscleVel;
    float64 *encoderData;
    float64 *muscleLengthPreviousTick;
    float64 *muscleLengthOffset;
    float64 *muscleEMG;
    float64 *spindleIa;
    float64 *spindleII;
    float64 *encoderBias;
    float64 *encoderGain;
    float64 *cortexDrive;
    
    int handleName;
    int No_of_musc;

    //tick is the time we start a new loop
    //tock is the time we start the PID
    double tick,tock;

    int expProtocol;
    int expProtocolAdvance;

    Muscles *muscleObj;
    // double motorCommand[3]; // moved this to motorControl.h
    //double errorForce[2];
    //double integral[2];
    //double EMG;
   

    //a temporary solution to data write to file VSync problem
    //have 2 buffers for the data that are need to file writing: back buffer and frame buffer
    //1.update the back buffer data
    //2.write frame buffer data to file and copy back buffer to frame buffer(separate thread)
    //however, the copy action may still slow the whole thing

    motorData();
    ~motorData(void);
    void initVariables();
};

