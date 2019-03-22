#include "dataOneSample.h"

dataOneSample::dataOneSample()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double loadCellData[7]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    TaskHandle loadCellHandle, encoderHandle[2];

    DAQmxErrChk (DAQmxCreateTask("",&loadCellHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai0" ,"loadCell0",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai8" ,"loadCell1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai1" ,"loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai9" ,"loadCell3",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai2" ,"loadCell4",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai10","loadCell5",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai11","loadCell6",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));    
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,3));

    DAQmxErrChk (DAQmxStartTask(loadCellHandle));
    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,3,NULL,NULL));

    loadCell0 = loadCellData[0] * loadCellScale0;
    loadCell1 = loadCellData[1] * loadCellScale1;
    loadCell2 = loadCellData[2] * loadCellScale2;
    loadCell3 = loadCellData[3] * loadCellScale3;
    loadCell4 = loadCellData[4] * loadCellScale4;
    loadCell5 = loadCellData[5] * loadCellScale5;
    loadCell6 = loadCellData[6] * loadCellScale6;

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
