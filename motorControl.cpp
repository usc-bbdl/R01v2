#include "motorControl.h"
#include <utilities.h>
#include <stdio.h>
motorControl::motorControl(double offset1, double offset2)
{
    I = 3;
    char        errBuff[2048]={'\0'};
    int32       error=0;
    newTrial = 0;
    gammaStatic1 = 0;
    gammaDynamic1 = 0;
    gammaStatic2 = 0;
    gammaDynamic2 = 0;
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
    loadCellData[0] = 0;
    loadCellData[1] = 0;
    motorRef[0] = 4;
    motorRef[1] = 4;
    encoderData1[0] = 0;
    encoderData2[0] = 0;
    resetMuscleLength = TRUE;
    muscleLengthPreviousTick[0] = 1;
    muscleLengthPreviousTick[1] = 1;
    muscleLengthOffset [0] = 0;
    muscleLengthOffset [1] = 0;
    strcpy(header,"Time, Muscle Length1, Muscle Length2, Load Cell1, Load Cell2");
    if (dataAcquisitionFlag[0]){
        strcat (header, ", FPGA Force1, FPGA Force2");
    }
    if (dataAcquisitionFlag[1]){
        strcat (header, ", EMG1, EMG2");
    }
    if (dataAcquisitionFlag[2]){
        strcat (header, ", SpindleIa1, SpindleIa2");
    }
    if (dataAcquisitionFlag[3]){
        strcat (header, ", SpindleII1, SpindleII2");
    }
    if (dataAcquisitionFlag[4]){
        strcat (header, ", Spike Count1, Spike Count2");
    }
    if (dataAcquisitionFlag[5]){
        strcat (header, ", Raster 1-1,  Raster 2-1");
    }
    if (dataAcquisitionFlag[6]){
        strcat (header, ", Raster 1-2,  Raster 2-2");
    }
    if (dataAcquisitionFlag[7]){
        strcat (header, ", Raster 1-3,  Raster 2-3");
    }
    if (dataAcquisitionFlag[8]){
        strcat (header, ", Raster 1-4,  Raster 2-4");
    }
    if (dataAcquisitionFlag[9]){
        strcat (header, ", Raster 1-5,  Raster 2-5");
    }
    if (dataAcquisitionFlag[10]){
        strcat (header, ", Raster 1-6,  Raster 2-6");
    }
    strcat(header,", Gamma Static, Gamma Dynamic, is Sample Missed, new Trial\n");

    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot7/ai1","loadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot7/ai0","loadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    DAQmxErrChk (DAQmxSetRealTimeConvLateErrorsToWarnings(loadCelltaskHandle,1));

    DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor1",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao10","motor2",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","motorEnable",DAQmx_Val_ChanForAllLines));

    
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[0]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[0],"PXI1Slot3/ctr4","Enoder 1",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot7/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[1]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[1],"PXI1Slot3/ctr5","Enoder 2",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[1],"/PXI1Slot7/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(motorTaskHandle);
		DAQmxClearTask(loadCelltaskHandle);
		DAQmxClearTask(motorEnableHandle);

		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor, load cell or encoder initialization error\n");
	}
}
motorControl::~motorControl()
{
    DAQmxClearTask(motorEnableHandle);
    DAQmxClearTask(motorTaskHandle);
    DAQmxClearTask(loadCelltaskHandle);
    live = FALSE;
}

int motorControl::motorEnable()
{
    uInt32      dataEnable=0x0000000f;
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 zeroVoltages[2]={0.0,0.0};

    DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,NULL,NULL));
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    Sleep(500);
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isEnable = TRUE;
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);

		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor enable Error\n");
	}
    return 0;
}

int motorControl::motorDisable()
{
	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;
    float64 zeroVoltages[2]={0.0,0.0};
    while(isControlling){
    }
    DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,NULL,NULL));
    
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    Sleep(500);
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isControlling = FALSE;
    isWindUp = FALSE;
    live = FALSE;
    isEnable = FALSE;

Error:
	if( DAQmxFailed(error) ){
		printf("DisableMotor Error: %s\n",errBuff);
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        printf("DAQmx Error: %s\n",errBuff);
		DAQmxStopTask(motorEnableHandle);
        DAQmxStopTask(motorTaskHandle);

        DAQmxClearTask(motorEnableHandle);
        DAQmxClearTask(motorTaskHandle);
    }
	return 0;
}
int motorControl::motorWindUp()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 windingUpCmnd[2]={0.5,0.5};
    if (isEnable){
        DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,windingUpCmnd,NULL,NULL));
        Sleep(500);
        isWindUp = TRUE;

        DAQmxStopTask(motorTaskHandle);
        
        printf("Windup Pass.\n");
    }else  printf("Motors must be first enabled prior to winding up.\n");
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
        DAQmxClearTask(motorTaskHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("winding up Error\n");
	}
     return 0;
}

void motorControl::motorControlLoop(void* a)
{
	((motorControl*)a)->controlLoop();
}

void motorControl::controlLoop(void)
{
    int32       error=0;
    bool keepReading=TRUE;
    bool32 isLate = {0};
    double tick=0.0,tock=0.0;
    float64 motorCommand[2]={0.0,0.0},errorForce[2]= {0.0,0.0},integral[2]={0.0,0.0};
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
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Motor Command1, Motor Command2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, motorRef1, motorRef2, spindleIa1, spindleIa2, spindleII1, spindleII2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Length 1, Length2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    fprintf(dataFile,header);
    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[0]));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[1]));
    timeData.resetTimer();
    tick = timeData.getCurrentTime();
    float64 goffsetLoadCell[2]={0};
    while(live)
    {
        WaitForSingleObject(hIOMutex, INFINITE);
        //desire Forced, muscle Length, muscle Velocity PIPES should be read here
        
        DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate));
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,2,NULL,NULL));
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[0],1,10.0,encoderData1,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[1],1,10.0,encoderData2,1,NULL,0));
        tock = timeData.getCurrentTime();
        if (resetMuscleLength)
        {
            muscleLengthOffset[0] = 2 * PI * shaftRadius * encoderData1[0] / 365;
            muscleLengthOffset[1] = 2 * PI * shaftRadius * encoderData1[1] / 365;

            resetMuscleLength = FALSE;
        }
        muscleLength[0] = ((2 * PI * shaftRadius * encoderData1[0] / 365) - muscleLengthOffset[0]);
        muscleLength[0] = 0.95 + (muscleLength[0] + 0.0059)*24.7178;
        muscleLength[1] = ((2 * PI * shaftRadius * encoderData2[0] / 365) - muscleLengthOffset[1]);
        muscleLength[1] = 0.95 + (muscleLength[1] - 0.0058)*24.4399;
        muscleVel[0] = (muscleLength[0] -  muscleLengthPreviousTick[0]) / (tock - tick);
        muscleVel[1] = (muscleLength[1] -  muscleLengthPreviousTick[1]) / (tock - tick);

        muscleLengthPreviousTick[0] = muscleLength[0];
        muscleLengthPreviousTick[1] = muscleLength[1];
        
        loadCellData[0] = (loadCellData[0] * loadCellScale1) - loadCellOffset1;
        loadCellData[1] = (loadCellData[1] * loadCellScale2) - loadCellOffset2;
        errorForce[0] = motorRef[0] - loadCellData[0];
        errorForce[1] = motorRef[1] - loadCellData[1];
        integral[0] = integral[0] + errorForce[0] * (tock - tick);
        integral[1] = integral[1] + errorForce[1] * (tock - tick);
        motorCommand[0] = integral[0] * I;
        motorCommand[1] = integral[1] * I;
        if (motorCommand[0] > motorMaxVoltage)
            motorCommand[0] = motorMaxVoltage;
        if (motorCommand[0] < motorMinVoltage)
            motorCommand[0] = motorMinVoltage;
        if (motorCommand[1] > motorMaxVoltage)
            motorCommand[1] = motorMaxVoltage;
        if (motorCommand[1] < motorMinVoltage)
            motorCommand[1] = motorMinVoltage;
        printf("Ld Cell1: %+6.2f; Enc 1: %+6.5f; Enc: %+6.5f, Dyn: %d, Sta: %d, \r",loadCellData[0],muscleLength[0],muscleLength[1],gammaDynamic1, gammaStatic1);
        ReleaseMutex( hIOMutex);
        //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d\n",tock,loadCellData[0],loadCellData[1],motorRef[0],motorRef[1], muscleLength[0], muscleLength[1], muscleVel[0],muscleVel[1], muscleEMG[0], muscleEMG[1], isLate);
        //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\n",tock,loadCellData[0],loadCellData[1], muscleLength[0], muscleLength[1], muscleVel[0],muscleVel[1], muscleEMG[0], muscleEMG[1], gammaStatic, gammaDynamic, isLate);
        //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\n",tock, muscleLength[0], muscleLength[1], muscleEMG[0], muscleEMG[1], gammaStatic, gammaDynamic, isLate);
        sprintf(dataSample,"%.3f,%.6f,%.6f,%.6f,%.6f",tock,muscleLength[0], muscleLength[1], loadCellData[0],loadCellData[1]);
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
            sprintf(dataTemp,",%d,%d",raster_MN_1[0], raster_MN_1[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[6]){
            sprintf(dataTemp,",%d,%d",raster_MN_2[0], raster_MN_2[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[7]){
            sprintf(dataTemp,",%d,%d",raster_MN_3[0], raster_MN_3[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[8]){
            sprintf(dataTemp,",%d,%d",raster_MN_4[0], raster_MN_4[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[9]){
            sprintf(dataTemp,",%d,%d",raster_MN_5[0], raster_MN_5[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[10]){
            sprintf(dataTemp,",%d,%d",raster_MN_6[0], raster_MN_6[1]);
            strcat (dataSample, dataTemp);
        }
        sprintf(dataTemp,",%d,%d,%d,%d,%d,%d\n",gammaStatic1, gammaDynamic1, gammaStatic2, gammaDynamic2, isLate,newTrial);
        if (newTrial == 1)
        {
            newTrial = 0;
        }
        strcat (dataSample, dataTemp);
        fprintf(dataFile,dataSample);
        tick = timeData.getCurrentTime();
    }
    isControlling = FALSE;
    DAQmxStopTask(motorTaskHandle);
    fclose(dataFile);
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(loadCelltaskHandle);
		DAQmxClearTask(loadCelltaskHandle);
        DAQmxStopTask(encodertaskHandle);
		DAQmxClearTask(encodertaskHandle);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor control Error\n");
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
    DAQmxStopTask(motorTaskHandle);
	DAQmxClearTask(motorTaskHandle);
    DAQmxStopTask(loadCelltaskHandle);
	DAQmxClearTask(loadCelltaskHandle);
    DAQmxStopTask(encodertaskHandle);
	DAQmxClearTask(encodertaskHandle);
    return 0;
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