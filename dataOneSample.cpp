#include "dataOneSample.h"

dataOneSample::dataOneSample()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double loadCellData[3]= {0.0,0.0,0.0};
    TaskHandle loadCellHandle, encoderHandle[2];

    DAQmxErrChk (DAQmxCreateTask("",&loadCellHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai0","loadCell0",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai8","loadCell1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));//old:ai9
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai1","loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));//old:ai9
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,3));

    DAQmxErrChk (DAQmxStartTask(loadCellHandle));
    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,3,NULL,NULL));

    printf("\n\nRaw LC Offsets:\n\t0: %2.4f, 1: %2.4f, 2: %2.4f .\n\n", loadCellData[0],loadCellData[1],loadCellData[2]);

    loadCell1 = loadCellData[0] * loadCellScale1;
    loadCell2 = loadCellData[1] * loadCellScale2;
    loadCell3 = loadCellData[2] * loadCellScale3;

    printf("\n\nScaled LC Offsets:\n\t0: %2.4f, 1: %2.4f, 2: %2.4f .\n\n", loadCell1,loadCell2,loadCell3);

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
