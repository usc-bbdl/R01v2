#pragma once

#include <stdio.h>
class motorData
{
private:
    double loadCellOffset1, loadCellOffset2,I;
    double encoderData1[1],encoderData2[1],muscleLengthPreviousTick[2], muscleLengthOffset[2];
    bool resetMuscleLength;
    double loadCellData[2],motorRef[2],muscleLength[2],muscleVel[2];
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    unsigned int muscleSpikeCount[2],raster_MN_1[2],raster_MN_2[2],raster_MN_3[2],raster_MN_4[2],raster_MN_5[2],raster_MN_6[2];
    float muscleEMG[2],spindleIa[2], spindleII[2],encoderBias[2],encoderGain[2];
    bool isEnable, isWindUp, isControlling;
    int gammaDynamic1, gammaStatic1,gammaDynamic2, gammaStatic2,trialTrigger;
    double cortexDrive[2], angle, velocity;

public:
    motorData(double offset1, double offset2);
    ~motorData(void);
};

