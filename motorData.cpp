#include "motorData.h"


motorData::motorData(double offset1, double offset2)
{
    handleName =0;
    //initialize stuff
    encoderBias[0] = encoderBias[1] = 0;
    encoderGain[0] = encoderGain[1] = 0;
    I = 4;
    cortexVoluntaryAmp = 10000;
    cortexVoluntaryFreq = 0.25;
    angle = 0;
    velocity = 0;
    trialTrigger = 0;
    gammaStatic1 = 0;
    gammaDynamic1 = 0;
    gammaStatic2 = 0;
    gammaDynamic2 = 0;
    cortexDrive[0] = 0;
    cortexDrive[1] = 0;
    spindleIa[0] = 0;
    spindleII[0] = 0;
    spindleIa[1] = 0;
    spindleII[1] = 0;
    loadCellOffset1 = offset1;
    loadCellOffset2 = offset2;
    loadCellData[0] = 0;
    loadCellData[1] = 0;
    motorRef[0] = 4;
    motorRef[1] = 4;
    encoderData1[0] = 0;
    encoderData2[0] = 0;
    resetMuscleLength = true;
    muscleLengthPreviousTick[0] = 1;
    muscleLengthPreviousTick[1] = 1;
    muscleLengthOffset [0] = 0;
    muscleLengthOffset [1] = 0;
}


motorData::~motorData(void)
{
}
