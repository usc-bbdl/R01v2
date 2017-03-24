#include "DAQ.h"


const char* loadCell_list[7] = 
{
    "",
    "ai8",
    "",
    "ai9",
    "",
    "",
    ""
};

const char* motorCmd_list[7] = 
{
    "",
    "ao9",
    "",
    "ao11",
    "",
    "",
    ""
};

uInt32 motorEnb_list[7] = 
{
    0x00000001,
    0xff, //default should be 0x02
    0x00000100,
    0xff, //default should be 0x04
    0x00010000,
    0x00100000,
    0x01000000
};

const char* motorEnc_list[7] = 
{
    "",
    "ctr7",
    "",
    "ctr3",
    "",
    "",
    "",
};

muscleAddress muscles;

muscleAddress::muscleAddress()
{
    loadCell = loadCell_list;
    motorCmd = motorCmd_list;
    motorEnb = motorEnb_list;
    motorEnc = motorEnc_list;
}

const char* muscleAddress::get_loadCell(int n)
{
    //std::cout<<"\nIn muscle.get_loadCell(): "<<loadCell[n]<<std::endl;
    return loadCell[n];
}

const char* muscleAddress::get_motorCmd(int n)
{

    return motorCmd[n];
}

uInt32 muscleAddress::get_motorEnb(int n)
{
    return motorEnb[n];
}

const char* muscleAddress::get_motorEnc(int n)
{
    return motorEnc[n];
}
//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

Muscles::Muscles()
{
}
Muscles::Muscles(int* musc,int M)
{
    No_of_musc = M;
    for(int i =0 ;i<MAX_MUS; i++) activeMuscles[i] = -1;
    EnbData = 0x00000000;
    LcTask = new Task();
    CmdTask = new Task();
    EnbTask = new Task();
    
    char* temp = new char[10];
    //std::cout<<"\n-->>initBuf";
    //char* buf = new char[10];

    //std::cout<<"\n"<<(sizeof(musc))<<(sizeof(*musc));
    for(int i=0;i<M;i++)
        {
            std::cout<<"\n"<<musc[i]<<"~~~\n";
            activeMuscles[musc[i]] = musc[i];
            EncTask[musc[i]] = new Task();
            strcpy(temp,"LC");
            //std::cout<<"\n"<<temp<<"~~~\n";
            sprintf(temp, "%s%d", temp,musc[i]);

            //std::cout<<"\n"<<temp<<"~~~\n";
            LcTask->assignTask(LcHandle,temp,musc[i],Lc);

            strcpy(temp,"Cmd");
            sprintf(temp, "%s%d", temp,musc[i]);

            //std::cout<<"\n"<<temp<<"~~~\n";
            CmdTask->assignTask(CmdHandle,temp,musc[i],Cmd);//


            //strcpy(temp,"Enc");
            sprintf(temp, "%s%d", temp,musc[i]);

            //std::cout<<"\n"<<temp<<"~~~\n";
            EncTask[musc[i]]->assignTask(EncHandle[musc[i]],temp,musc[i],Enc);//

            EnbData = EnbData | motorEnb_list[musc[i]];
           
            
        }

    strcpy(temp,"Enb");
    sprintf(temp, "%s%d",temp,(int) (sizeof(musc)/sizeof(*musc)));
    
    //std::cout<<"\n"<<temp<<"~~~\n";
    EnbTask->assignTask(EnbHandle,temp,0,Enb);
}

void Muscles::initMuscles()
{
    //std::cout<<"\ninitLC";
    LcTask->initTask();
    //std::cout<<"\ninitEnb";
    EnbTask->initTask();
    //std::cout<<"\ninitCmd";
    CmdTask->initTask();
    //std::cout<<"\ninitEnc";
    for(int i=0; i<MAX_MUS; i++)
        if(activeMuscles[i] != -1) EncTask[i]->initTask();
}

void Muscles::startMuscles()
{
    LcTask->startTask();
    EnbTask->startTask();
    CmdTask->startTask();
    for(int i=0; i<MAX_MUS; i++)
        if(activeMuscles[i] != -1) EncTask[i]->startTask();
}

void Muscles::stopMuscles()
{
    LcTask->stopTask();
    EnbTask->stopTask();
    CmdTask->stopTask();
    for(int i=0; i<MAX_MUS; i++)
        if(activeMuscles[i] != -1) EncTask[i]->stopTask();
}

void Muscles::deleteMuscles()
{
    LcTask->deleteTask();
    EnbTask->deleteTask();
    CmdTask->deleteTask();
    for(int i=0; i<MAX_MUS; i++)
        if(activeMuscles[i] != -1) EncTask[i]->deleteTask();
}

double* Muscles::MuscleLc()
{
    return LcTask->daqTask();
}

double* Muscles::MuscleEnc()
{
    double* temp = new double[MAX_MUS];
    int j=0;
    for(int i=0; i<MAX_MUS; i++) 
        if(activeMuscles[i] != -1) temp[j++] = (EncTask[i]->daqTask())[0];

    //double* ptr;
    //ptr = temp;
    return temp;
}

/*
void Task::daqTask(double* dataArg)
{daqObj->writeDAQ(task,dataArg);
}
void Task::daqTask(uInt32* dataArg)
{daqObj->writeDAQ(task,dataArg);
}
*/

void Muscles::MuscleCmd(double* dataArg)
{
    CmdTask->daqTask(dataArg);
}

void Muscles::zeroMuscles()
{
    double* dataArg = new double[No_of_musc];
    for(int i=0; i<No_of_musc; i++) dataArg[i] = 0.0;
    CmdTask->daqTask(dataArg);
}
void Muscles::MuscleEnb(int b)
{
    uInt32* temp = new uInt32;
    //temp[0] = 0x00;
    if(!b)
    {
        temp[0] = 0x00;
        EnbTask->daqTask(temp);
    }
    else if(b)
    {
        temp[0] = EnbData;
        EnbTask->daqTask(temp);
    }

}

//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------
Task::Task()
{
    task = NULL;
    handleName = "";
    slot = "";
    type = -1;
    slotNo=0;
    data = NULL;
    temp = new char[20];

    //daqObj = new DAQ();
}

Task::~Task(void)
{
    stopTask();
    deleteTask();
    task = NULL;
    handleName = "";
    slot = "";
    type = -1;
    slotNo=0;
    encNo=0;
    data = NULL;
    temp = NULL;
    delete daqObj;
    delete data;
    delete temp;
}

Task::Task(void* &taskArg,string handleNameArg, string slotArg, int typeArg)
{
    task = taskArg;
    handleName = handleNameArg;
    slot = slotArg;
    type = typeArg;
    temp = new char[20];
}

void Task::assignTask(void* &taskArg,string handleNameArg, string slotArg, int typeArg,int enc)
{
    handleName = handleNameArg;
    
    type = typeArg;    

    createTask(taskArg,handleName);
    task = taskArg;

    //std::cout<<"\nIn task.assignTask() "<<assignSlot(slotNo)<<" SlotNo:"<<slotNo<<std::endl;
    //strcpy((char*) slot,assignSlot(slotNo));
    if(typeArg == Enc) slotNo = enc; //to assign slotNo without a global variable
    slot = assignSlot(slotNo);
    //if(typeArg == Enc) std::cout<<"\nEnc slot: "<<slot<<"\n\n";
    createChan();
}

void Task::assignTask(void* &taskArg,string handleNameArg, int slotNo, int typeArg)
{
    handleName = handleNameArg;
    
    type = typeArg;    

    createTask(taskArg,handleName);
    task = taskArg;

    //std::cout<<"\nIn task.assignTask() "<<assignSlot(slotNo)<<" SlotNo:"<<slotNo<<std::endl;
    //strcpy((char*) slot,assignSlot(slotNo));
    //if(typeArg == Enc) slotNo = enc; //to assign slotNo without a global variable
    //if(typeArg == Enb) slot = ;
    slot = assignSlot(slotNo);
    //if(typeArg == Enc) std::cout<<"\nEnc slot: "<<slot<<"\n\n";
    createChan();
}

void Task::createTask(void* &taskArg,string handleNameArg)
{    
    if(task==NULL || type==Enc)
    {
        daqObj->createHandle(taskArg,handleNameArg);
        //slotNo++;
    }
}

char* Task::assignSlot(int n)
{
    if(type==Lc)
    {
        strcpy(temp,Lc_slot);
        strcat(temp,muscles.get_loadCell(n));
        //std::cout<<"\nInside Task.assignSlot(): "<<temp1<< "+" <<temp2<<std::endl;
        //temp1 = strcat(temp1,temp2);
        //std::cout<<"\nInside Task.assignSlot(): "<<temp1<<std::endl;
        return temp;
    }
    if(type==Cmd)
    {
        //char* temp = new char[20];
        strcpy(temp,Cmd_slot);
        strcat(temp,muscles.get_motorCmd(n));
        return temp;
    }
    if(type==Enc)
    {
        //char* temp = new char[20];
        strcpy(temp,Enc_slot);
        strcat(temp,muscles.get_motorEnc(n));
        //std::cout<<"\nAssigning Enc for slot: "<<temp<<std::endl;
        return temp;
    }
    if(type == Enb)
    {
        strcpy(temp,Enb_slot);
        //strcat(temp,muscles.get_motorEnb(n));
        return temp;
    }
    //return "empty";
}



void Task::createChan()
{
    daqObj->createChan(*this);
    slotNo++;
}

void Task::initTask(int samples)
{
    //std::cout<<"\nIn initTask: samples: "<<samples<<std::endl;
    daqObj->createClk(*this,samples);
}

void Task::startTask()
{
    daqObj->startHandle(task);
}

double* Task::daqTask(void)
{
    if(type==Lc)
        data = new double[slotNo];
    if(type==Enc && data==NULL)
        data = new double[2];
    //data = 0;

    if(type!=Lc && type!=Enc)
        return 0;
    
    daqObj->readDAQ(*this);
    return data;
}

/*
template <typename T> void Task::daqTask(T& dataArg)
{
    daqObj->writeDAQ(task,*dataArg);
    //DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));

    //DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,NULL,NULL));
}
*/

void Task::daqTask(double* dataArg)
{daqObj->writeDAQ(task,dataArg);
}
void Task::daqTask(uInt32* dataArg)
{daqObj->writeDAQ(task,dataArg);
}

/*
void Task::daqTask(uInt32* dataArg)
{
    daqObj->writeDAQ(task,dataArg);
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
    daqObj->writeDAQ(task,dataArg);
    DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,NULL,NULL));
}
*/

void Task::stopTask(){    daqObj->stopHandle(task);}

void Task::deleteTask(){    daqObj->clearHandle(task);}

void* Task::getHandle(){    return task;}

const char* Task::getName(){    return handleName;}

const char* Task::getSlot(){    return slot;}

int Task::getType(){    return type;}

double* Task::getData(){    return data;}

int Task::getSlotNo(){  return slotNo;}


//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

//------------------------------------------------
//------------------------------------------------

DAQ::DAQ(void)
{
    // get zeroing values
}


DAQ::~DAQ(void)
{
    // should assist in safe shutdown
}

void DAQ::createHandle(void* &taskArg,string handleNameArg)
{
     errorCheck(DAQmxCreateTask(handleNameArg,&taskArg),taskArg);
}

void DAQ::createChan(Task &taskObj)
{
    //std::cout<<"\nCreating channel for "<<taskObj.getName();
    TaskHandle task = taskObj.getHandle();
    //task = &task;
    if(taskObj.getType() == Lc)
        errorCheck(DAQmxCreateAIVoltageChan(taskObj.getHandle(),taskObj.getSlot(),taskObj.getName(),DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL),task);
    if(taskObj.getType() == Cmd)
        errorCheck(DAQmxCreateAOVoltageChan(taskObj.getHandle(),taskObj.getSlot(),taskObj.getName(),motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL),task);
    if(taskObj.getType() == Enb)
            errorCheck(DAQmxCreateDOChan(taskObj.getHandle(),taskObj.getSlot(),taskObj.getName(),DAQmx_Val_ChanForAllLines),task);
        //if(!taskObj.getHandle())
    if(taskObj.getType() == Enc)
        errorCheck(DAQmxCreateCIAngEncoderChan(taskObj.getHandle(),taskObj.getSlot(),taskObj.getName(),DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""),task);
}

void DAQ::createClk(Task &taskObj,int samples=1)
{
    TaskHandle task = taskObj.getHandle();
    //std::cout<<"\nIn createClk: samples: "<<samples<<" Slots:"<<taskObj.getSlotNo()<<std::endl;
    if(taskObj.getType() == Lc)
    {
        if(samples!=1)
        {
            //std::cout<<"\nSamples !=1 : yes";
            errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),"",500000.0,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,taskObj.getSlotNo()),task);//for dataOneSample
        }
        else
        {
            errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL),task);
            errorCheck (DAQmxSetRealTimeConvLateErrorsToWarnings(taskObj.getHandle(),1),task);
        }
    }
    if(taskObj.getType() == Cmd)
        errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,samples),task);
    if(taskObj.getType() == Enb)
    ;
    
    //DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    if(taskObj.getType() == Enc)
        errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),SampleClk,controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1),task);
}

void DAQ::startHandle(void* &taskArg)
{
    errorCheck(DAQmxStartTask(taskArg),taskArg);
}

void DAQ::readDAQ(Task &taskObj)
{
    bool32 isLate = {0};
    TaskHandle task = taskObj.getHandle();
    if(taskObj.getType() == Lc)
    {
        errorCheck(DAQmxWaitForNextSampleClock(task,10, &isLate),task);
        errorCheck(DAQmxReadAnalogF64(task,-1,10.0,DAQmx_Val_GroupByScanNumber,taskObj.getData(),2,NULL,NULL),task);
    }
    if(taskObj.getType() == Enc)
        errorCheck(DAQmxReadCounterF64(task,1,10.0,taskObj.getData(),1,NULL,0),task);
}

void DAQ::writeDAQ(void* &taskArg, double *dataArg)
{
    errorCheck(DAQmxWriteAnalogF64(taskArg,1,FALSE,10,DAQmx_Val_GroupByChannel,dataArg,NULL,NULL),taskArg);
}

void DAQ::writeDAQ(void* &taskArg, uInt32 *dataArg)
{
    errorCheck(DAQmxWriteDigitalU32(taskArg,1,1,10.0,DAQmx_Val_GroupByChannel,dataArg,NULL,NULL),taskArg);
}

void DAQ::stopHandle(void* &taskArg)
{
    errorCheck(DAQmxStopTask(taskArg),taskArg);
}

void DAQ::clearHandle(void* &taskArg)
{
    errorCheck(DAQmxClearTask(taskArg),taskArg);
}

void DAQ::errorCheck(int error, TaskHandle &handle)
{
    char errBuff[2048]={'\0'};
    if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		printf("DAQmx Error: %s\n",errBuff);
	if (handle)
    {
        DAQmxStopTask(handle);
		DAQmxClearTask(handle);
        handle = 0;
    }

    }
}