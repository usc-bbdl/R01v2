#include "dataOneSample.h"

dataOneSample::dataOneSample()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double loadCellData[2]= {0.0,0.0};
    TaskHandle loadCellHandle, encoderHandle[2];

    DAQmxErrChk (DAQmxCreateTask("",&loadCellHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai0","loadCell1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai1","loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));

    DAQmxErrChk (DAQmxStartTask(loadCellHandle));
    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,2,NULL,NULL));
    loadCell1 = loadCellData[0] * loadCellScale1;
    loadCell2 = loadCellData[1] * loadCellScale2;

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		printf("DAQmx Error: %s\n",errBuff);
	}
    if (loadCellHandle)
    {
        DAQmxStopTask(loadCellHandle);
		DAQmxClearTask(loadCellHandle);
        loadCellHandle = 0;
    }
}

dataOneSample::~dataOneSample()
{
}
