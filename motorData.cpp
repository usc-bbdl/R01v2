#include "motorData.h"


motorData::motorData(double offset1, double offset2)
{
    handleName =0;
    //initialize stuff
    // moving muscle declaration from motorControl constructor to here
    int musc[] = {1,3};
    //std::cout<<"\n"<<(sizeof(musc))<<(sizeof(*musc));
    No_of_musc = (sizeof(musc))/(sizeof(*musc));
    loadCellData = new double[No_of_musc];
    motorRef = new double[No_of_musc];
    muscleLength = new double[No_of_musc];
    muscleVel = new double[No_of_musc];
    muscleLengthOffset = new double[No_of_musc];
    encoderData = new double[No_of_musc];
    muscleLengthPreviousTick = new double[No_of_musc];
    encoderBias = new double[No_of_musc];
    muscleEMG = new double[No_of_musc];
    spindleIa  = new double[No_of_musc];
    spindleII = new double[No_of_musc];
    encoderBias = new double[No_of_musc];
    encoderGain = new double[No_of_musc];
    cortexDrive = new double[No_of_musc];

    initVariables(offset1, offset2);

}


motorData::~motorData(void)
{
}

void motorData::initVariables(double offset1, double offset2) {
    
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
    loadCellOffset[0] = offset1;
    loadCellOffset[1] = offset2;
    loadCellData[0] = 0;
    loadCellData[1] = 0;
    motorRef[0] = 4;
    motorRef[1] = 4;
    encoderData[0] = 0;
    encoderData[1] = 0;
    resetMuscleLength = true;
    muscleLengthPreviousTick[0] = 1;
    muscleLengthPreviousTick[1] = 1;
    muscleLengthOffset [0] = 0;
    muscleLengthOffset [1] = 0;

    motorCommand[0]=0;
    motorCommand[1]=0;
    motorCommand[2]=0;
    //errorForce[0]=0;
    //errorForce[1]=0;
    //integral[0]=0;
    //integral[1]=0;
    //EMG=0.0;

    tick=0.0;
    tock=0.0;

    expProtocol=0;
    expProtocolAdvance=0;
}