#include "DAQ.h"


const char* loadCell_list[7] = 
{
    "ai8",
    "ai9",
    "",
    "",
    "",
    "",
    ""
};

const char* motorCmd_list[7] = 
{
    "ao9",
    "ao11",
    "ao31",
    "",
    "",
    "",
    ""
};

uInt32 motorEnb_list[7] = 
{
    0x00000001,
    0x00000001, //default should be 0x02
    0x00000100,
    0x00001000,
    0x00010000,
    0x00100000,
    0x01000000
};

const char* motorEnc_list[7] = 
{
    "ctr3",
    "ctr7",
    "",
    "",
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


Muscles::Muscles(int musc[])
{
    for(int i=0;i<MAX_MUS;i++)
        if(musc[i])
        {

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

void Task::assignTask(void* &taskArg,string handleNameArg, string slotArg, int typeArg)
{
    handleName = handleNameArg;
    
    type = typeArg;    

    createTask(taskArg,handleName);
    task = taskArg;

    //std::cout<<"\nIn task.assignTask() "<<assignSlot(slotNo)<<" SlotNo:"<<slotNo<<std::endl;
    //strcpy((char*) slot,assignSlot(slotNo));
    slot = assignSlot(slotNo);
    createChan();
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

void Task::createTask(void* &taskArg,string handleNameArg)
{    
    if(task==NULL || type==Enc)
    {
        daqObj->createHandle(taskArg,handleNameArg);
        //slotNo++;
    }
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
        data = new double[1];
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
            errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),"",500000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,taskObj.getSlotNo()),task);//for dataOneSample
        }
        else
            errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL),task);
    }
    if(taskObj.getType() == Cmd)
        errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,samples),task);
    if(taskObj.getType() == Enb)
    ;
    if(taskObj.getType() == Enc)
        errorCheck(DAQmxCfgSampClkTiming(taskObj.getHandle(),SampleClk,controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,samples),task);
}

void DAQ::startHandle(void* &taskArg)
{
    errorCheck(DAQmxStartTask(taskArg),taskArg);
}

void DAQ::readDAQ(Task &taskObj)
{
    TaskHandle task = taskObj.getHandle();
    if(taskObj.getType() == Lc)
        errorCheck(DAQmxReadAnalogF64(task,-1,10.0,DAQmx_Val_GroupByScanNumber,taskObj.getData(),2,NULL,NULL),task);
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