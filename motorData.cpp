#include "motorData.h"


motorData::motorData()
{
    handleName =0;
    //initialize stuff
    // moving muscle declaration from motorControl constructor to here
    int musc[] = {1,3};
    No_of_musc = (sizeof(musc))/(sizeof(*musc));
    muscleObj = new Muscles(musc, (sizeof(musc))/(sizeof(*musc)));

    // TODO: check for type consistency, ie. float64 vs. double
    newPdgm_ref = new double[No_of_musc]; // consider moving this to motroControl
    encoderData = new double[No_of_musc];
    loadCellData = new double[No_of_musc];
    loadCellOffset = new double[No_of_musc];
    muscleLengthPreviousTick = new double[No_of_musc];
    muscleLengthOffset = new double[No_of_musc];
    encoderBias = new double[No_of_musc];
    encoderGain = new double[No_of_musc];
    motorRef = new double[No_of_musc];
    muscleLength = new double[No_of_musc];
    muscleVel = new double[No_of_musc];
    
    gammaStatic = new int[No_of_musc];
    gammaDynamic = new int[No_of_musc];
    cortexDrive = new double[No_of_musc];
    muscleSpikeCount = new int[No_of_musc];

    muscleEMG = new double[No_of_musc];
    spindleIa  = new double[No_of_musc];
    spindleII = new double[No_of_musc];
   
    raster_MN_1 = new int[No_of_musc];
    raster_MN_2 = new int[No_of_musc];
    raster_MN_3 = new int[No_of_musc];
    raster_MN_4 = new int[No_of_musc];
    raster_MN_5 = new int[No_of_musc];
    raster_MN_6 = new int[No_of_musc];

    initVariables();

}


motorData::~motorData(void)
{
    // TODO: deallocate all assigned memory
    delete[] newPdgm_ref; // consider moving this to motroControl
    delete[] encoderData;
    delete[] loadCellData;
    delete[] loadCellOffset;
    delete[] muscleLengthPreviousTick;
    delete[] muscleLengthOffset;
    delete[] encoderBias;
    delete[] encoderGain;
    delete[] motorRef;
    delete[] muscleLength;
    delete[] muscleVel;
    
    delete[] gammaStatic;
    delete[] gammaDynamic;
    delete[] cortexDrive;
    delete[] muscleSpikeCount;

    delete[] muscleEMG;
    delete[] spindleIa;
    delete[] spindleII;
   
    delete[] raster_MN_1;
    delete[] raster_MN_2;
    delete[] raster_MN_3;
    delete[] raster_MN_4;
    delete[] raster_MN_5;
    delete[] raster_MN_6;
}

void motorData::initVariables() {
    
    for (int i=0; i<No_of_musc; i++) {
        encoderGain[i] = 0;
        encoderBias[i] = 0;
        encoderData[i] = 0;
        newPdgm_ref[i] = 0;
        loadCellData[i] = 0;
        motorRef[i] = 0;
        muscleLength[i] = 0; 
        muscleVel[i] = 0;
        gammaStatic[i] = 0;
        gammaDynamic[i] = 0;
        cortexDrive[i] = 0;
        muscleEMG[i] = 0;
        spindleIa[i] = 0;
        spindleII[i] = 0;
        muscleLengthPreviousTick[i] = 1;
        // loadCellOffset[i] = offset[i];
        muscleLengthOffset[i] = 0;  
    }
    // experimental control parameters
    expProtocol = tick = tock = 0;
    expProtocolAdvance = 0;
    I = 4;
    cortexVoluntaryAmp = 10000;
    cortexVoluntaryFreq = 0.25;
    angle = 0;
    velocity = 0;
    trialTrigger = 0;

    // motorControl experiment commands
    // isEnable = false;
    // isWindUp = false;
    // isControlling = false;
    // live = false;
    // resetMuscleLength = true;
    // expProtocolRunningStateMachine = false;
}