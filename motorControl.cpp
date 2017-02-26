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
    //DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));

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
    uInt32      dataEnable=0xff;
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
        //hIOMutex = CreateMutex(NULL, FALSE, NULL);
		isControlling = TRUE;
		_beginthread(motorControl::motorControlLoop,0,this);
    }else
    {
        isControlling = FALSE;
        printf("Motors must be first enabled or wind up before closed-loop control.\n");
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
    float cotexDrive = 0.0;
    bool keepReading=TRUE;
    bool32 isLate = {0};
    
    char errBuff[2048]={'\0'};

    //start handles(load)
    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[0]));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[1]));
    timeData.resetTimer();
    //mData->tick = timeData.getCurrentTime();
    float64 goffsetLoadCell[2]={0};

    while(live)
    {
        //WaitForSingleObject(hIOMutex, INFINITE);

        mData->tick = timeData.getCurrentTime();

        //desire Forced, muscle Length, muscle Velocity PIPES should be read here
        
        //wait for force sensor clock
        DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate));
        //read from force sensor
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,mData->loadCellData,2,NULL,NULL));
        //write to motor(motorcommand is modified in PID in each loop)
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,mData->motorCommand,NULL,NULL));
        //read from encoder(2 taskhandles for 2 clocks)
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[0],1,10.0,mData->encoderData1,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[1],1,10.0,mData->encoderData2,1,NULL,0));

        mData->tock = timeData.getCurrentTime();
        mData->motorCommand[0]=PID(mData->motorCommand[0]);
        mData->motorCommand[1]=PID(mData->motorCommand[1]);

        double EMG=0;
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
        mData->motorCommand[2] = EMG;

        //comand line print
        printf("F1: %+6.2f; F2: %+6.2f;L1: %+6.2f; L2: %+6.2f;, Dyn: %d, Sta: %d, \r",mData->loadCellData[0],mData->loadCellData[1],mData->muscleLength[0],mData->muscleLength[1],mData->gammaDynamic1, mData->gammaStatic1);
        //ReleaseMutex(hIOMutex);

        //print one line
        mLogger->startthread(mLogger);

        //change expProtocol and expProtocolAdvance
        if (mData->trialTrigger == 1){
            mData->expProtocolAdvance = 1;
            mData->trialTrigger = 0;
        }
        if (mData->trialTrigger == 2){
            mData->expProtocolAdvance = 10;
            mData->trialTrigger = 0;
        }
        if (mData->trialTrigger == 3){
            mData->expProtocolAdvance = 11;
            mData->trialTrigger = 0;
        }
        mData->expProtocol = 0;
        switch(mData->expProtocolAdvance){
            case 1:
                mData->expProtocol = -1000;
                mData->expProtocolAdvance = 2;
                break;
            case 2:
                mData->expProtocol = mData->gammaDynamic1;
                mData->expProtocolAdvance = 3;
                break;
            case 3:
                mData->expProtocol = mData->gammaStatic1;
                mData->expProtocolAdvance = 4;
                break;
            case 4:
                mData->expProtocol =  mData->cortexDrive[0];
                mData->expProtocolAdvance = 5;
                break;
            case 5:
                mData->expProtocol = mData->gammaDynamic2;
                mData->expProtocolAdvance = 6;
                break;
            case 6:
                mData->expProtocol = mData->gammaStatic2;
                mData->expProtocolAdvance = 7;
                break;
            case 7:
                mData->expProtocol =  mData->cortexDrive[1];
                mData->expProtocolAdvance = 8;
                break;
            case 8: 
                mData->expProtocol = mData->angle;
                mData->expProtocolAdvance = 9;
                break;
            case 9:
                mData->expProtocol = mData->velocity;
                mData->expProtocolAdvance = 0;
                break;
            case 10:
                mData->expProtocol = -1;
                mData->expProtocolAdvance = 0;
                break;
            case 11:
                mData->expProtocol = -2;
                mData->expProtocolAdvance = 0;
                break;
        }

        //mData->tick = timeData.getCurrentTime();

    }
    isControlling = FALSE;
    DAQmxStopTask(motorTaskHandle);
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

double motorControl::PID(double motorcommand,int channelnum){
    
    double errorForce[2]={0,0};
    double integral[2]={0,0};

        //start PID
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
        mData->muscleVel[0] = (mData->muscleLength[0] -  mData->muscleLengthPreviousTick[0]) / (mData->tock - mData->tick);
        mData->muscleVel[1] = (mData->muscleLength[1] -  mData->muscleLengthPreviousTick[1]) / (mData->tock - mData->tick);

        mData->muscleLengthPreviousTick[0] = mData->muscleLength[0];
        mData->muscleLengthPreviousTick[1] = mData->muscleLength[1];
        
        mData->loadCellData[0] = (mData->loadCellData[0] * loadCellScale1) - mData->loadCellOffset1;
        mData->loadCellData[1] = (mData->loadCellData[1] * loadCellScale2) - mData->loadCellOffset2;
        errorForce[0] = mData->motorRef[0] - mData->loadCellData[0];
        errorForce[1] = mData->motorRef[1] - mData->loadCellData[1];
        integral[0] = integral[0] + errorForce[0] * (mData->tock - mData->tick);
        integral[1] = integral[1] + errorForce[1] * (mData->tock - mData->tick);
        mData->motorCommand[0] = integral[0] * mData->I;
        mData->motorCommand[1] = integral[1] * mData->I;
        
        if (mData->motorCommand[0] > motorMaxVoltage)
            mData->motorCommand[0] = motorMaxVoltage;
        if (mData->motorCommand[0] < motorMinVoltage)
            mData->motorCommand[0] = motorMinVoltage;
        if (mData->motorCommand[1] > motorMaxVoltage)
            mData->motorCommand[1] = motorMaxVoltage;
        if (mData->motorCommand[1] < motorMinVoltage)
            mData->motorCommand[1] = motorMinVoltage;
        //end of PID

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