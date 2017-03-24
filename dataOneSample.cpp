#include "dataOneSample.h"
#include "DAQ.h"
#include <conio.h>

dataOneSample::dataOneSample()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    double* loadCellData;//[2]= {0.0,0.0};
    
    
    TaskHandle loadCellHandle;
    Task *loadCell = new Task();
    loadCell->assignTask(loadCellHandle,"loadCell1",1,Lc);
    loadCell->assignTask(loadCellHandle,"loadCell2",3,Lc);
    loadCell->initTask(2);
    loadCell->startTask();
    loadCellData = loadCell->daqTask();
    loadCell1 = loadCellData[0] * loadCellScale[0];
    loadCell2 = loadCellData[1] * loadCellScale[1];
    loadCell->stopTask();
    loadCell->deleteTask();

    //////////////////////////////////////////
    /*
    TaskHandle demoHandle, demoHandle_[2];
    double* demoData, *demoData_;
    Task *demoTask, *demoTask_[2];

    //LC read sample test
    {
    demoTask = new Task();//
    demoTask->assignTask(demoHandle,"demoLoadCell1",1,Lc);//
    demoTask->assignTask(demoHandle,"demoLoadCell2",3,Lc);//
    demoTask->initTask();
    demoTask->startTask();
    demoData = demoTask->daqTask();
    std::cout<<"\nDemo Task1 Data:"<< demoData[0]<< " " << demoData[1]<<std::endl;
    demoTask->stopTask();
    demoTask->deleteTask();
    }

    //Enc read sample task
    {
    demoTask_[0] = new Task();//
    //demoTask_[1] = new Task();
    demoTask_[0]->assignTask(demoHandle_[0],"demoEnc1",1,Enc);//
    //demoTask_[1]->assignTask(demoHandle_[1],"demoEnc2","",Enc,1);
    std::cout<<"\nAssigned Enc tasks\n\n";
    demoTask_[0]->initTask();//
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
    EnbTask->assignTask(EnbHandle,"Enb1",1,Enb);//
    CmdTask->assignTask(CmdHandle,"Cmd1",1,Cmd);//
    CmdTask->assignTask(CmdHandle,"Cmd2",3,Cmd);//
    EnbTask->initTask();//
    CmdTask->initTask();//
    EnbTask->startTask();//
    CmdTask->startTask();//
    EnbData = new uInt32;
    EnbData[0] = 0x00010010;
    CmdData = new double[2];
    CmdData[0]  = 0.5; CmdData[1]  = 0.5;
    std::cout<<"\nDemo Task3 Data:"<< CmdData[0]<< " " << CmdData[1]<<std::endl;
    std::cout<<"\nDemo Task4 Data:"<< EnbData[0]<<std::endl;
    std::cout<<"\nStarting motor rot";
    EnbTask->daqTask(EnbData);//
    CmdTask->daqTask(CmdData);//
    Sleep(3000);
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

    */
    /////////////////////////////////////////
    double *demoData1, *demoData2, *demoData3;
    demoData1 = new double[2];
    demoData2 = new double[2];
    demoData3 = new double[2];

    Muscles *muscleObj;

    int musc[] = {3};

    muscleObj = new Muscles(musc,(sizeof(musc))/(sizeof(*musc)));
    
    muscleObj->startMuscles();

    demoData1[0] = 0.5; //demoData1[1] = 0.5;

    muscleObj->MuscleEnb(1);

    int count=0;

    while(!kbhit())
    {    
    demoData2 = muscleObj->MuscleLc();

    muscleObj->MuscleCmd(demoData1);

    demoData3 = muscleObj->MuscleEnc();

    //std::cout<<"Muscle Data: "<< count++ <<"LC1: " <<demoData2[0]<< " LC2:" << demoData2[1]<<" Enc1:"<< demoData3[0]<<" Enc2:" << demoData3[1]<<"\r";
    //printf("Muscle Data: %d LC1: %+6.2f; LC2: %+6.2f; Enc1: %6.0f; Enc2: %6.0f \r\r",count++,demoData2[0],demoData2[1],demoData3[0],demoData3[1]);
    printf("Muscle Data: %d LC1: %+6.2f; Enc1: %6.0f;  \r\r",count++,demoData2[0],demoData3[0]);
    }
    demoData1[0] = 0.0; //demoData1[1] = 0.0;
    muscleObj->MuscleCmd(demoData1);
    muscleObj->MuscleEnb(0);

    muscleObj->stopMuscles();
    muscleObj->deleteMuscles();

}

dataOneSample::~dataOneSample()
{
}

/*
    TaskHandle demoHandle, demoHandle_[2];
    double* demoData, *demoData_;
    Task *demoTask, *demoTask_[2];

    //LC read sample test
    {
    demoTask = new Task();//
    demoTask->assignTask(demoHandle,"demoLoadCell1",1,Lc);//
    demoTask->assignTask(demoHandle,"demoLoadCell2",3,Lc);//
    demoTask->initTask();
    demoTask->startTask();
    demoData = demoTask->daqTask();
    std::cout<<"\nDemo Task1 Data:"<< demoData[0]<< " " << demoData[1]<<std::endl;
    demoTask->stopTask();
    demoTask->deleteTask();
    }

    //Enc read sample task
    {
    demoTask_[0] = new Task();//
    //demoTask_[1] = new Task();
    demoTask_[0]->assignTask(demoHandle_[0],"demoEnc1",1,Enc);//
    //demoTask_[1]->assignTask(demoHandle_[1],"demoEnc2","",Enc,1);
    std::cout<<"\nAssigned Enc tasks\n\n";
    demoTask_[0]->initTask();//
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
    EnbTask->assignTask(EnbHandle,"Enb1",1,Enb);//
    CmdTask->assignTask(CmdHandle,"Cmd1",1,Cmd);//
    CmdTask->assignTask(CmdHandle,"Cmd2",3,Cmd);//
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
    Sleep(3000);
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
    */