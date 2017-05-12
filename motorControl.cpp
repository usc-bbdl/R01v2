#include "motorControl.h"
#include "hardwareConfiguration.h"
#include <utilities.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#define NEW_TRIAL 1
#define NEW_PERTURBATION 2
motorControl::motorControl()
{
    hardwareConfiguration hwdConfig;
    hwdConfig.getNumberOfMuscles(No_of_musc);
    int * musc;
    musc = new int[No_of_musc];
    hwdConfig.getMuscleNumbers(musc);
    //int musc[] = {1,3};
    //No_of_musc = (sizeof(musc))/(sizeof(*musc));
    muscleObj = new Muscles(musc,(sizeof(musc))/(sizeof(*musc)));
    createVariables();
    initializeVariables();
    mData = new motorData();
    createHeader4DataFile();
}
motorControl::~motorControl()
{
    muscleObj->deleteMuscles();
    live = FALSE;
}
void motorControl::createVariables()
{
    newPdgm_ref = new float64[No_of_musc];
    loadCellData = new float64[No_of_musc];
    muscleLengthPreviousTick = new float64[No_of_musc];
    muscleLengthOffset = new float64[No_of_musc];
    encoderBias = new float64[No_of_musc];
    encoderGain = new float64[No_of_musc];
    motorRef = new float64[No_of_musc];
    muscleLength = new float64[No_of_musc];
    muscleVel = new float64[No_of_musc];

    gammaStatic = new int[No_of_musc];
    gammaDynamic = new int[No_of_musc];
    cortexDrive = new int[No_of_musc];
    muscleSpikeCount  = new int[No_of_musc];

    muscleEMG = new float[No_of_musc];
    spindleIa = new float[No_of_musc];
    spindleII = new float[No_of_musc];
    
    raster_MN_1 = new int[No_of_musc];
    raster_MN_2 = new int[No_of_musc];
    raster_MN_3 = new int[No_of_musc];
    raster_MN_4 = new int[No_of_musc];
    raster_MN_5 = new int[No_of_musc];
    raster_MN_6 = new int[No_of_musc];
}
void motorControl::initializeVariables()
{
    //Muscle specific parameters
    for (int i=0;i<No_of_musc;i++)
    {
        encoderGain[i] = 0;
        encoderBias [i] = 0;
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
        muscleSpikeCount[i] = 0;
        raster_MN_1[i] = 0;
        raster_MN_2[i] = 0;
        raster_MN_3[i] = 0;
        raster_MN_4[i] = 0;
        raster_MN_5[i] = 0;
        raster_MN_6[i] = 0;
        muscleLengthPreviousTick[i] = 1;
        muscleLengthOffset [i] = 0;
    }
    //experimental control parameters
    newPdgm_Flag = false;
    expProtocol = tick = tock = 0;
    I = 4;
    cortexVoluntaryAmp = 10000;
    cortexVoluntaryFreq = 0.25;
    angle = 0;
    velocity = 0;
    trialTrigger = 0;
    isEnable = FALSE;
    isWindUp = FALSE;
    isControlling = FALSE;
    live = FALSE;
    resetMuscleLength = TRUE;
    expProtocolRunningStateMachine = false;
}
void motorControl::setDataAcquisitionFlag(bool flag[])
{
    for (int i = 0 ; i< 12; i++)
        this->dataAcquisitionFlag[i] = flag[i];
}

int motorControl::createWindingUpCommand()
{
    windingUpCmnd = new float64[No_of_musc];
    for (int i = 0; i < No_of_musc; i ++)
    {
        windingUpCmnd[i] = 0.5;
    }
    return 0;
}
int motorControl::motorEnable()
{
    muscleObj->startMuscles();
    muscleObj->MuscleEnb(1);
    muscleObj->zeroMuscles();
    muscleObj->stopMuscles();
    isEnable = TRUE;
    return 0;
}

int motorControl::motorDisable()
{
    muscleObj->startMuscles();
    muscleObj->MuscleEnb(0);
    muscleObj->zeroMuscles();
    muscleObj->stopMuscles();
    isControlling = FALSE;
    isWindUp = FALSE;
    live = FALSE;
    isEnable = FALSE;  
	return 0;
}
int motorControl::motorWindUp()
{
    createWindingUpCommand();
    if (isEnable){
        muscleObj->startMuscles();
        muscleObj->MuscleCmd(windingUpCmnd);
        Sleep(500);
        isWindUp = TRUE;
        muscleObj->stopMuscles();
       
        printf("Windup Pass.\n");
    }else  printf("Motors must be first enabled prior to winding up.\n");
   
     return 0;
}

void motorControl::motorControlLoop(void* a)
{
	((motorControl*)a)->controlLoop();
}

void motorControl::controlLoop(void)
{
    float cotexDrive = 0.0;
    bool keepReading=TRUE;
    bool32 isLate = {0};
    float64 * motorCommand, * errorForce, * integral, *loadCellOffset;
    motorCommand = new float64[No_of_musc];
    errorForce = new float64[No_of_musc];
    integral = new float64[No_of_musc];
    FILE *dataFile;
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char fileName[200];

    sprintf_s(
            fileName,
            "C:\\data\\realTimeData%4d_%02d_%02d_%02d_%02d_%02d.txt",
            timePtr->tm_year+1900, 
            timePtr->tm_mon+1, 
            timePtr->tm_mday, 
            timePtr->tm_hour, 
            timePtr->tm_min, 
            timePtr->tm_sec
            );
    dataFile = fopen(fileName,"w");
    fprintf(dataFile,header);

    muscleObj->startMuscles();
    timeData.resetTimer();
    tick = timeData.getCurrentTime();
    loadCellOffset = muscleObj->MuscleLc();
    
    while(live)
    {
        WaitForSingleObject(hIOMutex, INFINITE);        
        loadCellData = muscleObj->MuscleLc();// needs a better name than LC
        muscleObj->MuscleCmd(motorCommand);
        double* encData = new double[No_of_musc];
        encData = muscleObj->MuscleEnc();
        tock = timeData.getCurrentTime();
        if (resetMuscleLength)
        {
            for (int i=0;i<No_of_musc;i++)
                muscleLengthOffset[i] = 2 * PI * shaftRadius * encData[i] / 365;
            resetMuscleLength = FALSE;
        }
        for (int i=0;i<No_of_musc;i++)
        {
            muscleLength[i] = ((2 * PI * shaftRadius * encData[i] / 365) - muscleLengthOffset[i]);   
            muscleLength[i] = encoderBias[i] + muscleLength[i] * encoderGain[i];
            muscleVel[i] = (muscleLength[i] -  muscleLengthPreviousTick[i]) / (tock - tick);
            muscleLengthPreviousTick[i] = muscleLength[i];
            loadCellData[i] = (loadCellData[i] * loadCellScale[i]) - loadCellOffset[i];
            errorForce[i] = motorRef[i] - loadCellData[i];
            if(newPdgm_Flag)
                errorForce[i] = newPdgm_ref[i] - loadCellData[i];
            integral[i] = integral[i] + errorForce[i] * (tock - tick);
            motorCommand[i] = integral[i] * I;
            if (motorCommand[i] > motorMaxVoltage)
                motorCommand[i] = motorMaxVoltage;
            if (motorCommand[i] < motorMinVoltage)
                motorCommand[i] = motorMinVoltage;
        }
        
        // print some data to screen
        printf("LC1: %+6.2f; LC2: %+6.2f; ER1: %+6.2f; ER2: %+6.2f; MC1: %+6.2f, MC2: %+6.2f, \r",loadCellData[0],loadCellData[1],errorForce[0],errorForce[1],motorCommand[0], motorCommand[1]);        
        ReleaseMutex( hIOMutex);
        //Log data to file
        createDataSampleString();
        fprintf(dataFile,dataSample);
        tick = timeData.getCurrentTime();
    }
    isControlling = FALSE;
    muscleObj->stopMuscles();   
    fclose(dataFile);
}

void motorControl::createDataSampleString()
{
    char dataTemp[100]="";
    //Mandatory data log
    sprintf(dataSample,"%.3f,%d,",tock,expProtocol);
    for (int i=0; i < No_of_musc; i++)
    {
        sprintf(dataTemp,",%.6f",loadCellData[i]);
        strcat (dataSample, dataTemp);
    }
    for (int i=0; i < No_of_musc; i++)
    {
        sprintf(dataTemp,",%.6f",muscleLength[i]);
        strcat (dataSample, dataTemp);
    }
    //Optional data log
    if (dataAcquisitionFlag[0])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%.6f",motorRef[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[1])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%.6f",muscleEMG[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[2])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%.6f",spindleIa[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[3])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%.6f",spindleII[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[4])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%.6f",muscleSpikeCount[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[5])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%u",raster_MN_1[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[6])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%u",raster_MN_2[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[7])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%u",raster_MN_3[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[8])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%u",raster_MN_4[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[9])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%u",raster_MN_5[i]);
            strcat (dataSample, dataTemp);
        }
    }
    if (dataAcquisitionFlag[10])
    {
        for (int i=0; i < No_of_musc; i++)
        {
            sprintf(dataTemp,",%u",raster_MN_6[i]);
            strcat (dataSample, dataTemp);
        }
    }
    
    if (dataAcquisitionFlag[11])
    {
        for (int i=0; i < No_of_musc; i++)
            cortexDrive[i] = max((cortexVoluntaryAmp -0) * sin (2 * 3.1416 * cortexVoluntaryFreq * tick), 0);
    }
        
    sprintf(dataTemp,"\n");
    strcat (dataSample, dataTemp);
    setExperimentalProtocol();
}
void motorControl::setExperimentalProtocol(void)
{
    if ((trialTrigger != 0) | (expProtocolRunningStateMachine == true))
        proceedExperimentalProtocol();
    else
        expProtocol = 0;
}

void motorControl::proceedExperimentalProtocol(void)
{
    static int expProtocoAdvance = 0;
    static int muscleNumber = 0;
    if (trialTrigger == NEW_TRIAL){
        expProtocoAdvance = 1;
        trialTrigger = 0;
        expProtocolRunningStateMachine = true;
    }
    if (trialTrigger == NEW_PERTURBATION){
        expProtocoAdvance = 100;
        trialTrigger = 0;
    }
    expProtocol = 0;
    switch(expProtocoAdvance){
        case 1:
            expProtocol = -1000;
            expProtocoAdvance = 2;
            expProtocolRunningStateMachine = true;
            muscleNumber = 0;
            break;
        case 2:
            if (muscleNumber<=No_of_musc)
            {
                expProtocol = gammaDynamic[muscleNumber];
                muscleNumber++;
            }
            else
            {
                expProtocoAdvance = 3;
                muscleNumber = 0;
            }
            expProtocolRunningStateMachine = true;
            break;
        case 3:
            if (muscleNumber<=No_of_musc)
            {
                expProtocol = gammaStatic[muscleNumber];
                muscleNumber++;
            }
            else
            {
                expProtocoAdvance = 4;
                muscleNumber = 0;    
            }
            expProtocolRunningStateMachine = true;
            break;
        case 4:
            if (muscleNumber<=No_of_musc)
            {
                expProtocol = cortexDrive[muscleNumber];
                muscleNumber++;
            }
            else
            {
                expProtocoAdvance = 5;
                muscleNumber = 0;    
            }
            expProtocolRunningStateMachine = true;
            break;
        case 5: 
            expProtocol = angle;
            expProtocoAdvance = 6;
            expProtocolRunningStateMachine = true;
            break;
        case 6:
            expProtocol = velocity;
            expProtocolRunningStateMachine = false;
            expProtocoAdvance = 0;
            break;
        case 100:
            expProtocol = -1;
            expProtocoAdvance = 0;
            expProtocolRunningStateMachine = false;
            break;
        default:
            trialTrigger = 0;
            expProtocoAdvance = 0;    
    }
}
int motorControl::motorControllerStart()
{
    if ((isEnable) && (isWindUp))
    {
        isControlling = TRUE;
        live = TRUE;
        hIOMutex = CreateMutex(NULL, FALSE, NULL);
		isControlling = TRUE;
		_beginthread(motorControl::motorControlLoop,0,this);
    }else
    {
        isControlling = FALSE;
        printf("Motors must be first enabled or wind up before closed-loop control.\n");
    }
    return 0;
}
int motorControl::motorControllerEnd()
{
    live = FALSE;
    motorControl::motorDisable();
    isControlling = FALSE;
    muscleObj->stopMuscles();
    muscleObj->deleteMuscles();

    return 0;
}

double motorControl::getTime()
{
    return timeData.getCurrentTime();
}
TimeData::TimeData(void)
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&initialTick);
}


TimeData::~TimeData(void)
{
}

// Reset the timer
int TimeData::resetTimer(void)
{
    QueryPerformanceCounter(&initialTick);
    return 0;
}


// Get current time in seconds
double TimeData::getCurrentTime(void)
{
    QueryPerformanceCounter(&currentTick);
    actualTime = (double)(currentTick.QuadPart - initialTick.QuadPart);
    actualTime /= (double)frequency.QuadPart;
    return actualTime;
}

int motorControl::createHeader4DataFile()
{
    char dataTemp[20];
    strcpy(header,"Time, Experimental Paradigm,");
    //Length and force is mandatory
    for (int i=0; i < No_of_musc;i++)
    {
            sprintf(dataTemp,"Muscle Length %d,", i);
            strcat (header, dataTemp);
    }

    for (int i=0; i < No_of_musc;i++)
    {
            sprintf(dataTemp,"Force Measured %d,", i);
            strcat (header, dataTemp);
    }
    if (dataAcquisitionFlag[0]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Force Reference %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[1]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"EMG %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[2]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Spindle Ia %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[3]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Spindle II %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[4]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"SpikeCount %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[5]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Raster 1-M %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[6]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Raster 2-M %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[7]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Raster 3-M %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[8]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Raster 4-M %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[9]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Raster 5-M %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[10]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Raster 6-M %d,", i);
            strcat (header, dataTemp);
        }
    }
    if (dataAcquisitionFlag[11]){
        for (int i=0; i < No_of_musc;i++)
        {
            sprintf(dataTemp,"Real-Time Control Cortex %d,", i);
            strcat (header, dataTemp);
        }
    }
    sprintf(dataTemp,"\n");
    strcat (header, dataTemp);
    //char dataTemp[100]="";
    strcat(header,"\n");
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[0],dataAcquisitionFlag[1],dataAcquisitionFlag[2],dataAcquisitionFlag[3]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[4],dataAcquisitionFlag[5],dataAcquisitionFlag[6],dataAcquisitionFlag[7]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d\n",dataAcquisitionFlag[8],dataAcquisitionFlag[9],dataAcquisitionFlag[10],dataAcquisitionFlag[11]);
    strcat(header,dataTemp);
    return 0;
}
void motorControl::newTrial(int trialTrigger)
{
    this->trialTrigger = trialTrigger;
}
void motorControl::setEncoderCalibration(double *encoderGain, double *encoderBias)
{
    for (int i = 0; i<No_of_musc; i++)
    {
        this->encoderBias[i] = encoderBias[i];
        this->encoderGain[i] = encoderGain[i];
    }
}
void motorControl::setMuscleReferenceForce(double *motorRef)
{
    for (int i = 0; i<No_of_musc; i++)
        this->motorRef[i] = ((float64) motorRef[i]);
}
void motorControl::setMuscleReferenceForce(float *motorRef)
{
    for (int i = 0; i<No_of_musc; i++)
        this->motorRef[i] = ((float64) motorRef[i]);
}

void motorControl::setMuscleEMG(double *muscleEMG)
{
    for (int i = 0; i<No_of_musc; i++)
        this->muscleEMG[i] = ((float) muscleEMG[i]);
}
void motorControl::setMuscleEMG(float *muscleEMG)
{
    for (int i = 0; i<No_of_musc; i++)
        this->muscleEMG[i] = muscleEMG[i];
}

void motorControl::setSpindleIa(double *spindleIa)
{
    for (int i = 0; i<No_of_musc; i++)
        this->spindleIa[i] = ((float) spindleIa[i]);
}
void motorControl::setSpindleIa(float *spindleIa)
{
    for (int i = 0; i<No_of_musc; i++)
        this->spindleIa[i] = spindleIa[i];   
}

void motorControl::setSpindleII(double *spindleII)
{
    for (int i = 0; i<No_of_musc; i++)
        this->spindleII[i] = ((float) spindleII[i]);
}
void motorControl::setSpindleII(float *spindleII)
{
    for (int i = 0; i<No_of_musc; i++)
        this->spindleII[i] = spindleII[i];    
}

void motorControl::setSpindleGammaDynamic(int *gammaDynamic)
{
    for (int i = 0; i<No_of_musc; i++)
        this->gammaDynamic[i] = gammaDynamic[i];
}
void motorControl::setSpindleGammaStatic(int *gammaStatic)
{
    for (int i = 0; i<No_of_musc; i++)
        this->gammaStatic[i] = gammaStatic[i];    
}
void motorControl::setRaster1(int *raster_MN_1)
{
    for (int i = 0; i<No_of_musc; i++)
        this->raster_MN_1[i] = raster_MN_1[i];    
}
void motorControl::setRaster2(int *raster_MN_2)
{
    for (int i = 0; i<No_of_musc; i++)
        this->raster_MN_2[i] = raster_MN_2[i];
    
}
void motorControl::setRaster3(int *raster_MN_3)
{
    for (int i = 0; i<No_of_musc; i++)
        this->raster_MN_3[i] = raster_MN_3[i];
    
}
void motorControl::setRaster4(int *raster_MN_4)
{
    for (int i = 0; i<No_of_musc; i++)
        this->raster_MN_4[i] = raster_MN_4[i];
}
void motorControl::setRaster5(int *raster_MN_5)
{
    for (int i = 0; i<No_of_musc; i++)
        this->raster_MN_5[i] = raster_MN_5[i];
    
}
void motorControl::setRaster6(int *raster_MN_6)
{
    for (int i = 0; i<No_of_musc; i++)
        this->raster_MN_6[i] = raster_MN_6[i];
}
void motorControl::setMuscleSpikeCount(int *muscleSpikeCount)
{
    for (int i = 0; i<No_of_musc; i++)
        this->muscleSpikeCount[i] = muscleSpikeCount[i];
}
void motorControl::setPerturbationAngle(double angle)
{
    this->angle = angle;
}
void motorControl::setPerturbationVelocity(double velocity)
{
    this->velocity = velocity;   
}
void motorControl::getLoadCellData(double *loadCellData)
{
    for (int i = 0; i<No_of_musc; i++)
        loadCellData[i] = ((double)this->loadCellData[i]);
}
void motorControl::getMuscleLength(double *muscleLength)
{
    for (int i = 0; i<No_of_musc; i++)
        muscleLength[i] = ((double)this->muscleLength[i]);
}
void motorControl::getMuscleVelocity(double *muscleVel)
{
    for (int i = 0; i<No_of_musc; i++)
        muscleVel[i] = ((double)this->muscleVel[i]);
}
void motorControl::resetLength()
{
    resetMuscleLength = true;
}
