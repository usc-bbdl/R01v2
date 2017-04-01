#include "motorControl.h"
#include <utilities.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
motorControl::motorControl(double offset1, double offset2)
{
    expProtocol = 0;
    tick = 0;
    tock = 0;
    int musc[] = {1,3};
    //std::cout<<"\n"<<(sizeof(musc))<<(sizeof(*musc));
    No_of_musc = (sizeof(musc))/(sizeof(*musc));
    muscleObj = new Muscles(musc,(sizeof(musc))/(sizeof(*musc)));
    createVariables();
    initializeVariables();
    //
    newPdgm_Flag = false;
    newPdgm_ref[0]=newPdgm_ref[1]=0;
    //
    mData=new motorData(offset1,offset2);
    encoderBias[0] = encoderBias[1] = 0;
    encoderGain[0] = encoderGain[1] = 0;
    I = 4;
    cortexVoluntaryAmp = 10000;
    cortexVoluntaryFreq = 0.25;
    char        errBuff[2048]={'\0'};
    int32       error=0;
    angle = 0;
    velocity = 0;
    trialTrigger = 0;
    isEnable = FALSE;
    isWindUp = FALSE;
    isControlling = FALSE;
    live = FALSE;
    loadCellOffset1 = offset1;
    loadCellOffset2 = offset2;
    encoderData1[0] = 0;
    encoderData2[0] = 0;
    resetMuscleLength = TRUE;
    muscleLengthPreviousTick[0] = 1;
    muscleLengthPreviousTick[1] = 1;
    muscleLengthOffset [0] = 0;
    muscleLengthOffset [1] = 0;
    motorControl::createHeader4DataFile();
    createWindingUpCommand();
    
}
motorControl::~motorControl()
{
    muscleObj->deleteMuscles();

    live = FALSE;
}
void motorControl::createVariables()
{
    loadCellData = new double[No_of_musc];
    motorRef = new double[No_of_musc];
    muscleLength = new double[No_of_musc];
    muscleVel = new double[No_of_musc];
    gammaStatic = new int[No_of_musc];
    gammaDynamic = new int[No_of_musc];
    cortexDrive = new int[No_of_musc];
    muscleEMG = new int[No_of_musc];
    spindleIa = new int[No_of_musc];
    spindleII = new int[No_of_musc];
    muscleSpikeCount  = new int[No_of_musc];
    raster_MN_1 = new int[No_of_musc];
    raster_MN_2 = new int[No_of_musc];
    raster_MN_3 = new int[No_of_musc];
    raster_MN_4 = new int[No_of_musc];
    raster_MN_5 = new int[No_of_musc];
    raster_MN_6 = new int[No_of_musc];
}
void initializeVariables()
{
    for (int i=0;i<No_of_musc;i++)
    {
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
    }
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
    char        errBuff[2048]={'\0'};
    int32       error=0;
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
    int32       error=0;
    float cotexDrive = 0.0;
    bool keepReading=TRUE;
    bool32 isLate = {0};
    float64 motorCommand[No_of_musc]={0.0},errorForce[No_of_musc]= {0.0},integral[No_of_musc]={0.0};
    char        errBuff[2048]={'\0'};
    FILE *dataFile;
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char fileName[200];
    char dataTemp[100]="";
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
    float64 goffsetLoadCell[2]={0};
    
    
    while(live)
    {
        WaitForSingleObject(hIOMutex, INFINITE);
        
        loadCellData = muscleObj->MuscleLc();// needs a better name LC
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
            loadCellData[i] = (loadCellData[i] * loadCellScale[i]) - loadCellOffset1;
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
    sprintf(dataSmaple,"%.3f,%d,",tock,expProtocol);
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
    int expProtocoAdvance = 0;
    if (trialTrigger == 1){
        expProtocoAdvance = 1;
        trialTrigger = 0;
    }
    if (trialTrigger == 2){
        expProtocoAdvance = 10;
        trialTrigger = 0;
    }
    if (trialTrigger == 3){
        expProtocoAdvance = 11;
        trialTrigger = 0;
    }
    expProtocol = 0;
    switch(expProtocoAdvance){
        case 1:
            expProtocol = -1000;
            expProtocoAdvance = 2;
            break;
        case 2:
            expProtocol = gammaDynamic1;
            expProtocoAdvance = 3;
            break;
        case 3:
            expProtocol = gammaStatic1;
            expProtocoAdvance = 4;
            break;
        case 4:
            expProtocol =  cortexDrive[0];
            expProtocoAdvance = 5;
            break;
        case 5:
            expProtocol = gammaDynamic2;
            expProtocoAdvance = 6;
            break;
        case 6:
            expProtocol = gammaStatic2;
            expProtocoAdvance = 7;
            break;
        case 7:
            expProtocol =  cortexDrive[1];
            expProtocoAdvance = 8;
            break;
        case 8: 
            expProtocol = angle;
            expProtocoAdvance = 9;
            break;
        case 9:
            expProtocol = velocity;
            expProtocoAdvance = 0;
            break;
        case 10:
            expProtocol = -1;
            expProtocoAdvance = 0;
            break;
        case 11:
            expProtocol = -2;
            expProtocoAdvance = 0;
            break;
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
    char dataTemp[100]="";
    strcat(header,"\n");
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[0],dataAcquisitionFlag[1],dataAcquisitionFlag[2],dataAcquisitionFlag[3]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[4],dataAcquisitionFlag[5],dataAcquisitionFlag[6],dataAcquisitionFlag[7]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d\n",dataAcquisitionFlag[8],dataAcquisitionFlag[9],dataAcquisitionFlag[10],dataAcquisitionFlag[11]);
    strcat(header,dataTemp);
    return 0;
}