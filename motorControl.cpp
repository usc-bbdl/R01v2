#include "motorControl.h"
#include <utilities.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

//#include <process.h>

motorControl::motorControl(double offset1, double offset2)
{

    //initialize the data class(all constants relavent in the experiment)
    mData=new motorData(offset1,offset2);

    //initialize the error buffer and motorcontrol flags
    char        errBuff[2048]={'\0'};
    int32       error=0;
    live = FALSE;
    isEnable = false;
    isWindUp = false;
    isControlling = false;
   
    //initilize a logger(write data to file every clock)
    mLogger=new logger(mData);

    //construct data file header
    strcpy(header,"Time, Exp Prot, Len1, Len2, ForcMeas1, ForcMeas2,");
    if (dataAcquisitionFlag[0]){
        strcat (header, ", ForceDes1, ForceDes2");
    }
    if (dataAcquisitionFlag[1]){
        strcat (header, ", EMG1, EMG2");
    }
    if (dataAcquisitionFlag[2]){
        strcat (header, ", Ia1, Ia2");
    }
    if (dataAcquisitionFlag[3]){
        strcat (header, ", II1, II2");
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

    //second line of data file
    char dataTemp[100]="";
    strcat(header,"\n");
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[0],dataAcquisitionFlag[1],dataAcquisitionFlag[2],dataAcquisitionFlag[3]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[4],dataAcquisitionFlag[5],dataAcquisitionFlag[6],dataAcquisitionFlag[7]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d\n",dataAcquisitionFlag[8],dataAcquisitionFlag[9],dataAcquisitionFlag[10],dataAcquisitionFlag[11]);
    strcat(header,dataTemp);

    //create task loadcell(force sensor) & channel & clock
    //1 handle: 2 channel
    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai8","loadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai9","loadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai16","ACC z",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai17","ACC y",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai18","ACC x",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai19","Side LoadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai20","Side LoadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    DAQmxErrChk (DAQmxSetRealTimeConvLateErrorsToWarnings(loadCelltaskHandle,1));
    //DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));

    //create task motor & channel & clock
    //1 handle: 3 channel(2 motors+1 speaker)
    DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao9","motor1",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor2",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao31","speaker",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    
    //create task motor enable & channel
    //1 handle: 1 channel
    DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","motorEnable",DAQmx_Val_ChanForAllLines));

    //create encoders & channels & clocks
    //1 handle:1 channel; 1 handle: 1 channel
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[0]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[0],"PXI1Slot3/ctr7","Enoder 1",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[1]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[1],"PXI1Slot3/ctr3","Enoder 2",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[1],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
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
    uInt32      dataEnable=0x00000001;
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 zeroVoltages[3]={0.0,0.0,0.0},zeroVoltage={0.0};
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
    float64 zeroVoltages[3]={0.0,0.0,0.0};
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
    float64 windingUpCmnd[3]={0.5,0.5,0};
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

    //create data file
    FILE *dataFile;
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char fileName[200];
    char dataSample[600]="";//1 line of data
    char dataTemp[100]="";//additional data add to datasample
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
    //no longer write header here
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Motor Command1, Motor Command2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, motorRef1, motorRef2, spindleIa1, spindleIa2, spindleII1, spindleII2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Length 1, Length2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //write existed header to datafile
    fprintf(dataFile,header);

    //start handles(load)
    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[0]));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[1]));
    timeData.resetTimer();
    tick = timeData.getCurrentTime();
    float64 goffsetLoadCell[2]={0};
    int expProtocol = 0;
    int expProtocoAdvance = 0;
    while(live)
    {
        WaitForSingleObject(hIOMutex, INFINITE);
        //desire Forced, muscle Length, muscle Velocity PIPES should be read here
        
        //wait for force sensor clock
        DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate));
        //read from force sensor
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,mData->loadCellData,2,NULL,NULL));
        //write to motor
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
        //read from encoder(2 taskhandles)
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[0],1,10.0,mData->encoderData1,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[1],1,10.0,mData->encoderData2,1,NULL,0));

        if (dataAcquisitionFlag[1]){
            EMG = mData->muscleEMG[0];
            if (EMG > 6)
                EMG = 6;
            if (EMG < -6)
                EMG = -6;
        }
        else{
            EMG = 0;
        }

        //start PID
        tock = timeData.getCurrentTime();
        if (mData->resetMuscleLength)
        {
            mData->muscleLengthOffset[0] = 2 * PI * shaftRadius * mData->encoderData1[0] / 365;
            mData->muscleLengthOffset[1] = 2 * PI * shaftRadius * mData->encoderData2[0] / 365;
            mData->resetMuscleLength = FALSE;
        }
        mData->muscleLength[0] = ((2 * PI * shaftRadius * mData->encoderData1[0] / 365) - mData->muscleLengthOffset[0]);
        //muscleLength[0] = 0.95 + (muscleLength[0] + 0.0059)*24.7178;
        //muscleLength[0] = 0.95 + (muscleLength[0] + 0.0059)*40;
        mData->muscleLength[0] = mData->encoderBias[0] + mData->muscleLength[0] *mData->encoderGain[0];
        mData->muscleLength[1] = ((2 * PI * shaftRadius * mData->encoderData2[0] / 365) - mData->muscleLengthOffset[1]);
        //muscleLength[1] = 1 + (muscleLength[1] - 0.0058)*30 + 0.5;
        //muscleLength[1] = 0.95 + (muscleLength[1] - 0.0058)*24.4399;
        mData->muscleLength[1] = mData->encoderBias[1] + mData->muscleLength[1] *mData->encoderGain[1];
        mData->muscleVel[0] = (mData->muscleLength[0] -  mData->muscleLengthPreviousTick[0]) / (tock - tick);
        mData->muscleVel[1] = (mData->muscleLength[1] -  mData->muscleLengthPreviousTick[1]) / (tock - tick);

        mData->muscleLengthPreviousTick[0] = mData->muscleLength[0];
        mData->muscleLengthPreviousTick[1] = mData->muscleLength[1];
        
        mData->loadCellData[0] = (mData->loadCellData[0] * loadCellScale1) - mData->loadCellOffset1;
        mData->loadCellData[1] = (mData->loadCellData[1] * loadCellScale2) - mData->loadCellOffset2;
        errorForce[0] = mData->motorRef[0] - mData->loadCellData[0];
        errorForce[1] = mData->motorRef[1] - mData->loadCellData[1];
        integral[0] = integral[0] + errorForce[0] * (tock - tick);
        integral[1] = integral[1] + errorForce[1] * (tock - tick);
        motorCommand[0] = integral[0] * mData->I;
        motorCommand[1] = integral[1] * mData->I;
        motorCommand[2] = EMG;
        if (motorCommand[0] > motorMaxVoltage)
            motorCommand[0] = motorMaxVoltage;
        if (motorCommand[0] < motorMinVoltage)
            motorCommand[0] = motorMinVoltage;
        if (motorCommand[1] > motorMaxVoltage)
            motorCommand[1] = motorMaxVoltage;
        if (motorCommand[1] < motorMinVoltage)
            motorCommand[1] = motorMinVoltage;
        //end of PID

        //comand line print
        printf("F1: %+6.2f; F2: %+6.2f;L1: %+6.2f; L2: %+6.2f;, Dyn: %d, Sta: %d, \r",mData->loadCellData[0],mData->loadCellData[1],mData->muscleLength[0],mData->muscleLength[1],mData->gammaDynamic1, mData->gammaStatic1);
        ReleaseMutex( hIOMutex);


/*
HANDLE WINAPI CreateThread(
__in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
__in SIZE_T dwStackSize,
__in LPTHREAD_START_ROUTINE lpStartAddress, 
__in_opt LPVOID lpParameter, 
__in DWORD dwCreationFlags,
__out_opt LPDWORD lpThreadId 
); 
*/
        mLogger->update(tick,tock,expProtocol);
        mLogger->startthread(mLogger);

        //output to data file
        //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d\n",tock,loadCellData[0],loadCellData[1],motorRef[0],motorRef[1], muscleLength[0], muscleLength[1], muscleVel[0],muscleVel[1], muscleEMG[0], muscleEMG[1], isLate);
        //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\n",tock,loadCellData[0],loadCellData[1], muscleLength[0], muscleLength[1], muscleVel[0],muscleVel[1], muscleEMG[0], muscleEMG[1], gammaStatic, gammaDynamic, isLate);
        //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\n",tock, muscleLength[0], muscleLength[1], muscleEMG[0], muscleEMG[1], gammaStatic, gammaDynamic, isLate);
        //construct 1 line of data(datasample)
        sprintf(dataSample,"%.3f,%d,%.6f,%.6f,%.6f,%.6f",tock,expProtocol,mData->muscleLength[0], mData->muscleLength[1], mData->loadCellData[0],mData->loadCellData[1]);
        //add additional data to the line
        if (dataAcquisitionFlag[0]){
            sprintf(dataTemp,",%.6f,%.6f",mData->motorRef[0],mData->motorRef[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[1]){
            sprintf(dataTemp,",%.6f,%.6f",mData->muscleEMG[0], mData->muscleEMG[1]);
            strcat (dataSample, dataTemp);
        }
         if (dataAcquisitionFlag[2]){
            sprintf(dataTemp,",%.6f,%.6f",mData->spindleIa[0], mData->spindleIa[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[3]){
            sprintf(dataTemp,",%.6f,%.6f",mData->spindleII[0], mData->spindleII[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[4]){
            sprintf(dataTemp,",%d,%d",mData->muscleSpikeCount[0], mData->muscleSpikeCount[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[5]){
            sprintf(dataTemp,",%u,%u",mData->raster_MN_1[0], mData->raster_MN_1[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[6]){
            sprintf(dataTemp,",%u,%u",mData->raster_MN_2[0], mData->raster_MN_2[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[7]){
            sprintf(dataTemp,",%u,%u",mData->raster_MN_3[0], mData->raster_MN_3[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[8]){
            sprintf(dataTemp,",%u,%u",mData->raster_MN_4[0], mData->raster_MN_4[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[9]){
            sprintf(dataTemp,",%u,%u",mData->raster_MN_5[0], mData->raster_MN_5[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[10]){
            sprintf(dataTemp,",%u,%u",mData->raster_MN_6[0], mData->raster_MN_6[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[11]){
            mData->cortexDrive[0] = max((mData->cortexVoluntaryAmp -0) * sin (2 * 3.1416 * mData->cortexVoluntaryFreq * tick), 0);
            mData->cortexDrive[1] = max((mData->cortexVoluntaryAmp -0) * sin (2 * 3.1416 * mData->cortexVoluntaryFreq * tick + 3.1416), 0);
        }
        //sprintf(dataTemp,",%d,%d,%d,%d,%.3f,%.3f,%d\n",gammaStatic1, gammaDynamic1, gammaStatic2, gammaDynamic2, cortexDrive[0], cortexDrive[1],newTrial);
        sprintf(dataTemp,"\n");
        
        //dont understand
        if (mData->trialTrigger == 1){
            expProtocoAdvance = 1;
            mData->trialTrigger = 0;
        }
        if (mData->trialTrigger == 2){
            expProtocoAdvance = 10;
            mData->trialTrigger = 0;
        }
        if (mData->trialTrigger == 3){
            expProtocoAdvance = 11;
            mData->trialTrigger = 0;
        }
        expProtocol = 0;
        switch(expProtocoAdvance){
            case 1:
                expProtocol = -1000;
                expProtocoAdvance = 2;
                break;
            case 2:
                expProtocol = mData->gammaDynamic1;
                expProtocoAdvance = 3;
                break;
            case 3:
                expProtocol = mData->gammaStatic1;
                expProtocoAdvance = 4;
                break;
            case 4:
                expProtocol =  mData->cortexDrive[0];
                expProtocoAdvance = 5;
                break;
            case 5:
                expProtocol = mData->gammaDynamic2;
                expProtocoAdvance = 6;
                break;
            case 6:
                expProtocol = mData->gammaStatic2;
                expProtocoAdvance = 7;
                break;
            case 7:
                expProtocol =  mData->cortexDrive[1];
                expProtocoAdvance = 8;
                break;
            case 8: 
                expProtocol = mData->angle;
                expProtocoAdvance = 9;
                break;
            case 9:
                expProtocol = mData->velocity;
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
        //add "\n" to datasample
        strcat (dataSample, dataTemp);

        //add 1 line(datasample) to the file
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

//--------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------

void motorControl::dummy()
{
    bool32 isLate = {0};
    char        errBuff[2048]={'\0'};
    int32       error=0;

    while(1){
        DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate);
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,mData->loadCellData,2,NULL,NULL));

        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[0],1,10.0,mData->encoderData1,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[1],1,10.0,mData->encoderData2,1,NULL,0));

        printf("F1: %+6.2f; F2: %+6.2f;L1: %+6.2f; L2: %+6.2f;, Dyn: %d, Sta: %d, \r",mData->loadCellData[0],mData->loadCellData[1],mData->muscleLength[0],mData->muscleLength[1]);
    }

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