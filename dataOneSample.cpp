#include "dataOneSample.h"
#include "DAQ.h"


dataOneSample::dataOneSample()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double* loadCellData;//[2]= {0.0,0.0};
    
    
    TaskHandle loadCellHandle;
    Task *loadCell = new Task();
    //std::cout<<"\nloadCell1";
    loadCell->assignTask(loadCellHandle,"loadCell1","PXI1Slot5/ai8",Lc);
    //std::cout<<"\nloadCell2";
    loadCell->assignTask(loadCellHandle,"loadCell2","PXI1Slot5/ai2",Lc);
    //std::cout<<"\nFinished Assign";
    loadCell->initTask(2);
    //std::cout<<"\nFinished Init";
    loadCell->startTask();
    //std::cout<<"\nFinished Start";
    //DAQmxErrChk (DAQmxReadAnalogF64(loadCellHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,2,NULL,NULL));
    loadCellData = loadCell->daqTask();
    //std::cout<<"\nFinished Read";    
    loadCell1 = loadCellData[0] * loadCellScale1;
    loadCell2 = loadCellData[1] * loadCellScale2;
    loadCell->stopTask();
    loadCell->deleteTask();

    
    TaskHandle demoHandle, demoHandle_[2];
    double* demoData, *demoData_;
    Task *demoTask, *demoTask_[2];

    //LC read sample test
    {
    demoTask = new Task();
    demoTask->assignTask(demoHandle,"demoLoadCell1","",Lc);
    demoTask->assignTask(demoHandle,"demoLoadCell2","",Lc);
    demoTask->initTask();
    demoTask->startTask();
    demoData = demoTask->daqTask();
    std::cout<<"\nDemo Task1 Data:"<< demoData[0]<< " " << demoData[1]<<std::endl;
    demoTask->stopTask();
    demoTask->deleteTask();
    }

    //Enc read sample task
    {
    demoTask_[0] = new Task();
    //demoTask_[1] = new Task();
    demoTask_[0]->assignTask(demoHandle_[0],"demoEnc1","",Enc,0);
    //demoTask_[1]->assignTask(demoHandle_[1],"demoEnc2","",Enc,1);
    std::cout<<"\nAssigned Enc tasks\n\n";
    demoTask_[0]->initTask();
    //demoTask_[1]->initTask();
    std::cout<<"\nInit Enc tasks\n\n";
    demoTask_[0]->startTask();
    //demoTask_[1]->startTask();
    std::cout<<"\nStart Enc tasks\n\n";
    demoData_ = demoTask_[0]->daqTask();
    demoData[0] = demoData_[0];
    std::cout<<"\nDaq Enc1 tasks\n\n";
    //demoData_ = demoTask_[1]->daqTask();
    demoData[1] = demoData_[0];
    std::cout<<"\nDaq Enc2 tasks\n";
    std::cout<<"\nDemo ENC Data:"<< demoData[0]<< " " << demoData[1]<<std::endl<<"\n";
    demoTask_[0]->stopTask();
    demoTask_[0]->deleteTask();
    std::cout<<"\nStop Enc1 tasks\n\n";
    //demoTask_[1]->stopTask();
    //demoTask_[1]->deleteTask();
    std::cout<<"\nStop Enc2 tasks\n\n";
    }

    TaskHandle EnbHandle;
    Task *EnbTask;
    uInt32* EnbData;
    TaskHandle CmdHandle;
    Task *CmdTask;
    double* CmdData;
    //Cmd & Enb test
    {
    EnbTask = new Task();
    CmdTask = new Task();
    EnbTask->assignTask(EnbHandle,"Enb1","",Enb);//
    CmdTask->assignTask(CmdHandle,"Cmd1","",Cmd);//
    CmdTask->assignTask(CmdHandle,"Cmd2","",Cmd);//
    EnbTask->initTask();//
    CmdTask->initTask();//
    EnbTask->startTask();//
    CmdTask->startTask();//
    EnbData = new uInt32;
    EnbData[0] = 0xff;
    CmdData = new double[2];
    CmdData[0]  = 0.5; CmdData[1]  = 0.5;
    std::cout<<"\nDemo Task3 Data:"<< CmdData[0]<< " " << CmdData[1]<<std::endl;
    std::cout<<"\nDemo Task4 Data:"<< EnbData[0]<<std::endl;
    std::cout<<"\nStarting motor rot";
    EnbTask->daqTask(EnbData);//
    CmdTask->daqTask(CmdData);//
    Sleep(2000);
    EnbData[0] = 0x00;
    CmdData[0]  = 0.0; CmdData[1]  = 0.0;
    std::cout<<"\nStopping motor rot";
    CmdTask->daqTask(CmdData);//
    EnbTask->daqTask(EnbData);//
    Sleep(100);
    std::cout<<"\nEnding motor operation";
    CmdTask->stopTask();//
    CmdTask->deleteTask();//
    std::cout<<"\nDelted CmdTask";
    EnbTask->stopTask();//
    EnbTask->deleteTask();//
    std::cout<<"\nDelted EnbTask";
    }
    //DAQmxStopTask(loadCellHandle);
	//DAQmxClearTask(loadCellHandle);

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
