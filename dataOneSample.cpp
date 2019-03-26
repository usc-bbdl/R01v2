#include "dataOneSample.h"

dataOneSample::dataOneSample()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double loadCellData[NUM_ANALOG_IN];
    TaskHandle loadCellHandle, encoderHandle[2];

    unsigned int i = 0;

    for(i = 0; i < MUSCLE_NUM + NUM_JR3_CHANNELS; i++) {
        loadCellData[i] = 0.0;            
    }
    
    DAQmxErrChk (DAQmxCreateTask("",&loadCellHandle));
    // load cells
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai0" ,"loadCell0",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai8" ,"loadCell1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai1" ,"loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai9" ,"loadCell3",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai2" ,"loadCell4",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai10","loadCell5",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai11","loadCell6",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    // JR3
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai17","JR3-FX",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai20","JR3-FY",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai19","JR3-FZ",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai16","JR3-MX",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai18","JR3-MY",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai21","JR3-MZ",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));

    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle, NULL, 1000,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));

    DAQmxErrChk (DAQmxStartTask(loadCellHandle));
    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData, NUM_ANALOG_IN ,NULL,NULL));

    printf("\n\nRaw LC Offsets:\n\t0: %2.4f, 1: %2.4f, 2: %2.4f, 3: %2.4f,\n\t4: %2.4f, 5: %2.4f, 6: %2.4f .\n\n",
        loadCellData[0],loadCellData[1],loadCellData[2],loadCellData[3],loadCellData[4],loadCellData[5],loadCellData[6]);
    for(i = 0; i < MUSCLE_NUM; i++) {
        loadCell[i] = loadCellData[i] * loadCellScale[i];
    }
    printf("\n\nScaled LC Offsets:\n\t0: %2.4f, 1: %2.4f, 2: %2.4f, 3: %2.4f,\n\t4: %2.4f, 5: %2.4f, 6: %2.4f .\n\n",
        loadCell[0], loadCell[1], loadCell[2], loadCell[3], loadCell[4], loadCell[5], loadCell[6] );

    for(i = MUSCLE_NUM; i < NUM_ANALOG_IN; i++) {
        JR3_V[i-MUSCLE_NUM] = loadCellData[i];
    }

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
