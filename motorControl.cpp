#include "motorControl.h"
motorControl::motorControl()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    isEnable = FALSE;
    isWindUp = FALSE;
    isControlled = FALSE;
    DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao10","motor1",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor2",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1));
    DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","enable07",DAQmx_Val_ChanForAllLines));
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
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
	DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
   	DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,&written,NULL));
    isEnable = TRUE;
Error:
	if( DAQmxFailed(error) ){
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		printf("EnableMotor Error: %s\n",errBuff);
        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);
    }
    return 0;
}

int motorControl::motorDisable()
{
	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;
    
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,&written,NULL));
    isEnable = FALSE;
    isWindUp = FALSE;
Error:
	if( DAQmxFailed(error) ){
		printf("DisableMotor Error: %s\n",errBuff);
		DAQmxStopTask(motorEnableHandle);
	    DAQmxClearTask(motorEnableHandle);

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
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,-1,DAQmx_Val_GroupByChannel,windingUpCmnd,NULL,NULL));
        DAQmxStopTask(motorTaskHandle);
        isWindUp = TRUE;
    }else  printf("Motors must be enbled first before winding up.");
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
int motorControl::motorController()
{
    //SURAJ WILL COMPLETE THIS SECTION
    return 0;
}
