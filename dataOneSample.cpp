#include "dataOneSample.h"

dataOneSample::dataOneSample()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double loadCellData[2]= {0.0,0.0};
    TaskHandle loadCellHandle, encoderHandle[2];

    DAQmxErrChk (DAQmxCreateTask("",&loadCellHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai8","loadCell1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai9","loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
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
    /*
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double loadCellData[2]= {0.0,0.0}, loadCellData1[2]= {0.0,0.0}, loadCellData2[2]= {0.0,0.0};
    TaskHandle loadCellHandle, encoderHandle[2], loadCellHandle1, loadCellHandle2;
    bool32 isLate = {0};
    std::cout<<"\nVar dec";
    DAQmxErrChk (DAQmxCreateTask("Task Handle 1",&loadCellHandle1));
    std::cout<<"\nHandle1 create task";
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle1,"PXI1Slot5/ai8","loadCell1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    std::cout<<"\nHandle1 create chan";
    //DAQmx_SwitchChan_AnlgBusSharingEnable
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle,"PXI1Slot5/ai9","loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    //DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle1,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    std::cout<<"\nHandle1 cfg clk";
    DAQmxErrChk (DAQmxSetRealTimeConvLateErrorsToWarnings(loadCellHandle1,1));
    std::cout<<"\nHandle1 set error";

    DAQmxErrChk (DAQmxCreateTask("Task Handle 1",&loadCellHandle2));
    std::cout<<"\nHandle2 create task";
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle2,"PXI1Slot5/ai9","loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    std::cout<<"\nHandle2 create chan";
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle2,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    std::cout<<"\nHandle2 cfg clk";

    Sleep(500);

    DAQmxErrChk (DAQmxStartTask(loadCellHandle1));
    std::cout<<"\nHandle1 start task";
    DAQmxErrChk (DAQmxStartTask(loadCellHandle2));
    std::cout<<"\nHandle2 start task";

    Sleep(500);

    DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCellHandle1,10, &isLate));
    std::cout<<"\nHandle1 waiting for clk";

    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle1,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData1,1,NULL,NULL));
    std::cout<<"\nHandle1 read data";
    loadCell1 = loadCellData1[0] * loadCellScale1;
    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle2,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData2,1,NULL,NULL));
    loadCell2 = loadCellData2[0] * loadCellScale2;
    std::cout<<"\nHandle2 read data";

    DAQmxStopTask(loadCellHandle1);
    DAQmxClearTask(loadCellHandle1);
    std::cout<<"\nHandle1 stop";
    DAQmxStopTask(loadCellHandle2);
    DAQmxClearTask(loadCellHandle2);    
    std::cout<<"\nHandle2 stop";
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        //
		printf("DAQmx Error: %s\n",errBuff);
	}
    if (loadCellHandle1 || loadCellHandle2)
    {
        DAQmxStopTask(loadCellHandle1);
        DAQmxClearTask(loadCellHandle1);
        std::cout<<"\nHandle1 stop";
        DAQmxStopTask(loadCellHandle2);
        DAQmxClearTask(loadCellHandle2);    
        std::cout<<"\nHandle2 stop";
        loadCellHandle1 = 0;
        loadCellHandle2 = 0;
    }
    */
    //---------------------------------------------
    /*
    double loadCellData[2]= {0.0,0.0};
    double loadCell[1] = {0.0};
    TaskHandle loadCellHandle1,loadCellHandle2, encoderHandle[2];

    DAQmxErrChk (DAQmxCreateTask("",&loadCellHandle1));
    DAQmxErrChk (DAQmxCreateTask("",&loadCellHandle2));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle1,"PXI1Slot5/ai8","loadCell1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCellHandle2,"PXI1Slot5/ai9","loadCell2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle1,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCellHandle2,"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1));

    DAQmxErrChk (DAQmxStartTask(loadCellHandle1));
    DAQmxErrChk (DAQmxStartTask(loadCellHandle2));
    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle1,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCell,1,NULL,NULL));
    loadCellData[0]= loadCell[0];
    DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle2,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCell,1,NULL,NULL));
    loadCellData[1]= loadCell[0];
    loadCell1 = loadCellData[0] * loadCellScale1;
    loadCell2 = loadCellData[1] * loadCellScale2;

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		printf("DAQmx Error: %s\n",errBuff);
	}
    if (loadCellHandle1)
    {
        DAQmxStopTask(loadCellHandle1);
		DAQmxClearTask(loadCellHandle1);
        loadCellHandle1 = 0;
    }
    if (loadCellHandle2)
    {
        DAQmxStopTask(loadCellHandle2);
		DAQmxClearTask(loadCellHandle2);
        loadCellHandle2 = 0;
    }
    */
}

dataOneSample::~dataOneSample()
{
}
