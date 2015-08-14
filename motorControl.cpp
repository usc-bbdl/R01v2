#include "motorControl.h"
#include <utilities.h>
//#include "pthread.h"

motorControl::motorControl(double offset1, double offset2)
{
    char        errBuff[2048]={'\0'};
    int32       error=0;

    I = 7;
    isEnable = FALSE;
    isWindUp = FALSE;
    loadCellOffset1 = offset1;
    loadCellOffset2 = offset2;
    isControlling = FALSE;
    DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor1",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao10","motor2",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","motorEnable",DAQmx_Val_ChanForAllLines));

    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot7/ai0","loadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot7/ai1","loadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    DAQmxErrChk (DAQmxSetRealTimeConvLateErrorsToWarnings(loadCelltaskHandle,1));

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);

        DAQmxStopTask(loadCelltaskHandle);
		DAQmxClearTask(loadCelltaskHandle);

        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);

		printf("DAQmx Error: %s\n",errBuff);
        printf("Callback Error\n");
        printf("Motor initialization Error\n");
	}
}
motorControl::~motorControl()
{
}

int motorControl::motorEnable()
{
	int32       error=0;
	char        errBuff[2048]={'\0'};
    //uInt32      dataEnable=0xffffffff;
    uInt32      dataEnable=0x0000000f;
    //uInt32      dataEnable=0xff000000;
    int32		written;
    float64 zeroVoltages[2]={0.0,0.0};
	DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
   	DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,&written,NULL));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    DAQmxStopTask(motorTaskHandle);
    isEnable = TRUE;
Error:
	if( DAQmxFailed(error) ){
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		printf("EnableMotor Error: %s\n",errBuff);
        DAQmxStopTask(motorEnableHandle);
    }
    return 0;
}

int motorControl::motorDisable()
{
	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;
    //DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,&written,NULL));
    isEnable = FALSE;
    isWindUp = FALSE;
    isControlling = FALSE;
Error:
	if( DAQmxFailed(error) ){
		printf("DisableMotor Error: %s\n",errBuff);
		DAQmxStopTask(motorEnableHandle);
        DAQmxStopTask(motorTaskHandle);
        DAQmxStopTask(loadCelltaskHandle);

        DAQmxClearTask(motorEnableHandle);
        DAQmxClearTask(motorTaskHandle);
        DAQmxClearTask(loadCelltaskHandle);
    }
	return 0;
}
int motorControl::motorWindUp()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 windingUpCmnd[2]={0.5,0.5};
    float64 dataTemp[2] ={0.0,0.0} ;
    if (isEnable){

        DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
        bool32 isLate = {0};
        Sleep(500);
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,windingUpCmnd,NULL,NULL));
        Sleep(500);
        DAQmxStopTask(motorTaskHandle);
        isWindUp = TRUE;
        printf("Windup Pass.\n");
    }else  printf("Motors must be first enabled prior to winding up.\n");
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
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
    float64 loadCellData[2]={0.0,0.0},motorRef[2]={1.0,1.0},motorCommand[2]={0.0,0.0},errorForce[2]={0.0,0.0},integral[2]={0.0,0.0};
    double tick=0.0,tock=0.0;
    char        errBuff[2048]={'\0'};

    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    
    timeData.resetTimer();
    tick = timeData.getCurrentTime();
    float64 goffsetLoadCell[2]={0};
    while(isControlling)
    {
        WaitForSingleObject(hIOMutex, INFINITE);
        //motorRef pipe should be read here
        DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate));
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,2,NULL,NULL));
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
        tock = timeData.getCurrentTime();
        loadCellData[0] = (loadCellData[0] *loadCellScale1) - loadCellOffset1;
        loadCellData[1] = (loadCellData[1] *loadCellScale2) - loadCellOffset2;
        errorForce[0] = motorRef[0]-loadCellData[0];
        errorForce[1] = motorRef[1]-loadCellData[1];
        integral[0] = integral[0] + errorForce[0]*(tock-tick);
        integral[1] = integral[1] + errorForce[1]*(tock-tick);
        motorCommand[0] = integral[0] * I;
        motorCommand[1] = integral[1] * I;
        if (motorCommand[0]>motorMaxVoltage)
            motorCommand[0] = motorMaxVoltage;
        if (motorCommand[0]<motorMinVoltage)
            motorCommand[0] = motorMinVoltage;
        if (motorCommand[1]>motorMaxVoltage)
            motorCommand[1] = motorMaxVoltage;
        if (motorCommand[1]<motorMinVoltage)
            motorCommand[1] = motorMinVoltage;
        printf("Ld Cell1: %+6.2f; Ld Cell2: %+6.2fp; M Cmnd 1: %+6.2f;M Cmnd 2: %+6.2f\r",loadCellData[0],loadCellData[1],motorCommand[0],motorCommand[1]);
        ReleaseMutex( hIOMutex);
        tick = timeData.getCurrentTime();
    }
    DAQmxStopTask(loadCelltaskHandle);
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(loadCelltaskHandle);
		DAQmxClearTask(loadCelltaskHandle);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("Callback Error\n");
	}
}

int motorControl::motorControllerStart()
{
    if ((isEnable) && (isWindUp))
    {
        isControlling = TRUE;
        hIOMutex = CreateMutex(NULL, FALSE, NULL);
		isControlling = TRUE;
		_beginthread(motorControl::motorControlLoop,0,this);
        //int ctrl_handle_Controller = pthread_create(&PIDThread, NULL, motorControl::motorControlLoop, NULL);
    }else
    {
        isControlling = FALSE;
        printf("Motors must be first enabled or wind up before closed-loop control.\n");
    }
    return 0;
}
int motorControl::motorControllerEnd()
{
    //pthread_cancel(PIDThread);
    motorControl::motorDisable();
    isControlling = FALSE;
    DAQmxStopTask(motorTaskHandle);
	DAQmxClearTask(motorTaskHandle);
    DAQmxStopTask(loadCelltaskHandle);
	DAQmxClearTask(loadCelltaskHandle);
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