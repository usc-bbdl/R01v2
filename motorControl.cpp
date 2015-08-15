#include "motorControl.h"
#include <utilities.h>
//#include "pthread.h"

motorControl::motorControl(double offset1, double offset2)
{
    I = 5;
    isEnable = FALSE;
    isWindUp = FALSE;
    loadCellOffset1 = offset1;
    loadCellOffset2 = offset2;
    isControlling = FALSE;
}
motorControl::~motorControl()
{
}

int motorControl::motorEnable()
{
	char        errBuff[2048]={'\0'};
    int32       error=0;

    
    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot7/ai0","loadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot7/ai1","loadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    DAQmxErrChk (DAQmxSetRealTimeConvLateErrorsToWarnings(loadCelltaskHandle,1));

    DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor1",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao10","motor2",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"/PXI1Slot7/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","motorEnable",DAQmx_Val_ChanForAllLines));

    
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[0]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[0],"PXI1Slot3/ctr4","Enoder 1",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,500,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot7/ai/SampleClock",1000,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[0]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[0],"PXI1Slot3/ctr5","Enoder 2",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,500,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot7/ai/SampleClock",1000,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));


    //uInt32      dataEnable=0xffffffff;
    uInt32      dataEnable=0x0000000f;
    //uInt32      dataEnable=0xff000000;
    //int32		written;
    float64 zeroVoltages[2]={0.0,0.0};
	DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
   	DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,NULL,NULL));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    DAQmxStopTask(motorTaskHandle);
    isEnable = TRUE;
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
    return 0;
}

int motorControl::motorDisable()
{
	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;
    float64 zeroVoltages[2]={0.0,0.0};
    //DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    isControlling = FALSE;
    Sleep(200);
    //DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,&written,NULL));
    isEnable = FALSE;
    isWindUp = FALSE;
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    DAQmxStopTask(motorTaskHandle);
	DAQmxClearTask(motorTaskHandle);
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
    float64 loadCellData[2],motorRef[2]={8.4,6.8},motorCommand[2]={0.5,0.5},errorForce[2]={0.0,0.0},integral[2]={0.0,0.0},encoderData1[1]={0.0},encoderData2[1]={0.0};
    double tick=0.0,tock=0.0;
    char        errBuff[2048]={'\0'};
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
    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[0]));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[1]));
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
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[0],1,10.0,encoderData1,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[1],1,10.0,encoderData2,1,NULL,0));
        encoderData1[0] = 2 * PI * shaftRadius * encoderData1[0] / 365 + 1;
        encoderData2[0] = 2 * PI * shaftRadius * encoderData2[0] / 365 + 1;

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
        //printf("Ld Cell1: %+6.2f; Ld Cell2: %+6.2fp; M Cmnd 1: %+6.2f;M Cmnd 2: %+6.2f, isLate%d\r",loadCellData[0],loadCellData[1],motorCommand[0],encoderData,isLate);
        printf("Ld Cell1: %+6.2f; Ld Cell2: %+6.2fp; Enc 1: %+6.5f; Enc: %+6.5f\r",loadCellData[0],loadCellData[1],encoderData1[0],encoderData2[0]);
        ReleaseMutex( hIOMutex);
        fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d\n",tock,loadCellData[0],loadCellData[1],motorCommand[0],motorCommand[1],encoderData1[0],encoderData2[0] ,isLate);
        tick = timeData.getCurrentTime();
    }
    fclose(dataFile);
    DAQmxStopTask(loadCelltaskHandle);
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
    //DAQmxStopTask(motorTaskHandle);
	//DAQmxClearTask(motorTaskHandle);
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