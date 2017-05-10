#pragma once

#include <stdio.h>
#include <utilities.h>


class motorData
{
public:
    /* these were private before */
    double loadCellOffset[2],I;
    //double encoderData[2];
    //double muscleLengthPreviousTick[2], muscleLengthOffset[2];
    bool resetMuscleLength;
    //double loadCellData[2],motorRef[2],muscleLength[2],muscleVel[2];
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    unsigned int muscleSpikeCount[2],raster_MN_1[2],raster_MN_2[2],raster_MN_3[2],raster_MN_4[2],raster_MN_5[2],raster_MN_6[2];
    //float muscleEMG[2],spindleIa[2], spindleII[2],encoderBias[2],encoderGain[2];
    int gammaDynamic1, gammaStatic1,gammaDynamic2, gammaStatic2,trialTrigger;
    double angle, velocity;
    /* end of private list */
    /* pointer variables for use with DAQ class */
    float64 *loadCellData;
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

    double motorCommand[3];
    //double errorForce[2];
    //double integral[2];
    //double EMG;

    //tick is the time we start a new loop
    //tock is the time we start the PID
    double tick,tock;

    int expProtocol;
    int expProtocolAdvance;

    //a temporary solution to data write to file VSync problem
    //have 2 buffers for the data that are need to file writing: back buffer and frame buffer
    //1.update the back buffer data
    //2.write frame buffer data to file and copy back buffer to frame buffer(separate thread)
    //however, the copy action may still slow the whole thing

    motorData(double offset1, double offset2);
    ~motorData(void);
    void initVariables(double offset1, double offset2);
};

