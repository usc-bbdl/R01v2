#include "motorControl.h"
#include <utilities.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
motorControl::motorControl(double offset1, double offset2)
{

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
    spindleIa[0] = 0;
    spindleII[0] = 0;
    spindleIa[1] = 0;
    spindleII[1] = 0;
    isEnable = FALSE;
    isWindUp = FALSE;
    isControlling = FALSE;
    live = FALSE;
    loadCellOffset1 = offset1;
    loadCellOffset2 = offset2;
    std::cout<<"\n_______________________\nLoad cell offset1: "<<loadCellOffset1<<"\n_______________________\nLoad cell offset2: "<<loadCellOffset2<<std::endl;
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
motorControl::~motorControl()
{
    muscleObj->deleteMuscles();

    live = FALSE;
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
    double tick=0.0,tock=0.0;
    float64 motorCommand[3]={0.0,0.0,0.0},errorForce[2]= {0.0,0.0},integral[2]={0.0,0.0},EMG={0.0};
    char        errBuff[2048]={'\0'};
    FILE *dataFile;
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char fileName[200];
    char dataSample[600]="";
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
    int expProtocol = 0;
    int expProtocoAdvance = 0;
    while(live)
    {
        WaitForSingleObject(hIOMutex, INFINITE);
        
        loadCellData = muscleObj->MuscleLc();
        muscleObj->MuscleCmd(motorCommand);
        double* encData = new double[No_of_musc];
        encData = muscleObj->MuscleEnc();
        encoderData1[0] = encData[0];
        encoderData2[0] = encData[1];

        if (dataAcquisitionFlag[1]){
            EMG = muscleEMG[0];
            if (EMG > 6)
                EMG = 6;
            if (EMG < -6)
                EMG = -6;
        }
        else
            EMG = 0;
        tock = timeData.getCurrentTime();
        if (resetMuscleLength)
        {
            muscleLengthOffset[0] = 2 * PI * shaftRadius * encoderData1[0] / 365;
            muscleLengthOffset[1] = 2 * PI * shaftRadius * encoderData2[0] / 365;
            resetMuscleLength = FALSE;
        }
        muscleLength[0] = ((2 * PI * shaftRadius * encoderData1[0] / 365) - muscleLengthOffset[0]);
        //muscleLength[0] = 0.95 + (muscleLength[0] + 0.0059)*24.7178;
        //muscleLength[0] = 0.95 + (muscleLength[0] + 0.0059)*40;
        muscleLength[0] = encoderBias[0] + muscleLength[0] *encoderGain[0];
        muscleLength[1] = ((2 * PI * shaftRadius * encoderData2[0] / 365) - muscleLengthOffset[1]);
        //muscleLength[1] = 1 + (muscleLength[1] - 0.0058)*30 + 0.5;
        //muscleLength[1] = 0.95 + (muscleLength[1] - 0.0058)*24.4399;
        muscleLength[1] = encoderBias[1] + muscleLength[1] *encoderGain[1];
        muscleVel[0] = (muscleLength[0] -  muscleLengthPreviousTick[0]) / (tock - tick);
        muscleVel[1] = (muscleLength[1] -  muscleLengthPreviousTick[1]) / (tock - tick);

        muscleLengthPreviousTick[0] = muscleLength[0];
        muscleLengthPreviousTick[1] = muscleLength[1];
        
        loadCellData[0] = (loadCellData[0] * loadCellScale[0]) - loadCellOffset1;
        loadCellData[1] = (loadCellData[1] * loadCellScale[1]) - loadCellOffset2;
        errorForce[0] = motorRef[0] - loadCellData[0];
        errorForce[1] = motorRef[1] - loadCellData[1];
        if(newPdgm_Flag)
        {
            errorForce[0] = newPdgm_ref[0] - loadCellData[0];
            errorForce[1] = newPdgm_ref[1] - loadCellData[1];
        }

        integral[0] = integral[0] + errorForce[0] * (tock - tick);
        integral[1] = integral[1] + errorForce[1] * (tock - tick);
        motorCommand[0] = integral[0] * I;
        motorCommand[1] = integral[1] * I;
        motorCommand[2] = EMG;
        if (motorCommand[0] > motorMaxVoltage)
            motorCommand[0] = motorMaxVoltage;
        if (motorCommand[0] < motorMinVoltage)
            motorCommand[0] = motorMinVoltage;
        if (motorCommand[1] > motorMaxVoltage)
            motorCommand[1] = motorMaxVoltage;
        if (motorCommand[1] < motorMinVoltage)
            motorCommand[1] = motorMinVoltage;
        printf("LC1: %+6.2f; LC2: %+6.2f; ER1: %+6.2f; ER2: %+6.2f; MC1: %+6.2f, MC2: %+6.2f, \r",loadCellData[0],loadCellData[1],errorForce[0],errorForce[1],motorCommand[0], motorCommand[1]);
        
        ReleaseMutex( hIOMutex);
        
        sprintf(dataSample,"%.3f,%d,%.6f,%.6f,%.6f,%.6f",tock,expProtocol,muscleLength[0], muscleLength[1], loadCellData[0],loadCellData[1]);
        if (dataAcquisitionFlag[0]){
            sprintf(dataTemp,",%.6f,%.6f",motorRef[0],motorRef[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[1]){
            sprintf(dataTemp,",%.6f,%.6f",muscleEMG[0], muscleEMG[1]);
            strcat (dataSample, dataTemp);
        }
         if (dataAcquisitionFlag[2]){
            sprintf(dataTemp,",%.6f,%.6f",spindleIa[0], spindleIa[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[3]){
            sprintf(dataTemp,",%.6f,%.6f",spindleII[0], spindleII[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[4]){
            sprintf(dataTemp,",%d,%d",muscleSpikeCount[0], muscleSpikeCount[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[5]){
            sprintf(dataTemp,",%u,%u",raster_MN_1[0], raster_MN_1[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[6]){
            sprintf(dataTemp,",%u,%u",raster_MN_2[0], raster_MN_2[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[7]){
            sprintf(dataTemp,",%u,%u",raster_MN_3[0], raster_MN_3[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[8]){
            sprintf(dataTemp,",%u,%u",raster_MN_4[0], raster_MN_4[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[9]){
            sprintf(dataTemp,",%u,%u",raster_MN_5[0], raster_MN_5[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[10]){
            sprintf(dataTemp,",%u,%u",raster_MN_6[0], raster_MN_6[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[11]){
            cortexDrive[0] = max((cortexVoluntaryAmp -0) * sin (2 * 3.1416 * cortexVoluntaryFreq * tick), 0);
            cortexDrive[1] = max((cortexVoluntaryAmp -0) * sin (2 * 3.1416 * cortexVoluntaryFreq * tick + 3.1416), 0);
        }
        
        sprintf(dataTemp,"\n");
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
        strcat (dataSample, dataTemp);
        fprintf(dataFile,dataSample);
        tick = timeData.getCurrentTime();

    }
    isControlling = FALSE;
    muscleObj->stopMuscles();
    
    fclose(dataFile);
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