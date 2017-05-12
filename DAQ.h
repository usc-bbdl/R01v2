#pragma once
#ifndef DAQ_H
#define DAQ_H

#include "NIDAQmx.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <string.h>
#include "utilities.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

#define Lc 0  //refers load cell - force transducer //AI
#define Enb 1 //refers enable pin of muscle //DO
#define Cmd 2 //refers motor driver //AO
#define Enc 3 //refers encoder //CI
static char* Lc_slot = "PXI1Slot5/";
static char* Cmd_slot = "PXI1Slot2/";
static char* Enb_slot ="PXI1Slot2/port0";
static char* Enc_slot ="PXI1Slot3/";
static char* SampleClk = "/PXI1Slot5/ai/SampleClock";
#define MAX_NUM_MUSCLES 7

class muscleAddress
{
    typedef const char* string;
    string *loadCell;
    string *motorCmd;
    uInt32 *motorEnb;
    string *motorEnc;
public:
    muscleAddress();
    string get_loadCell(int);
    string get_motorCmd(int);
    uInt32 get_motorEnb(int);
    string get_motorEnc(int);
    //should contain physical addresses, in arrays for now
};

extern muscleAddress muscles;
class Task;
class Muscles
{
    typedef const char* string;

    //initialize with the number of muscles
    TaskHandle LcHandle;
    TaskHandle CmdHandle;
    TaskHandle EnbHandle;
    TaskHandle EncHandle[MAX_NUM_MUSCLES];
    Task *LcTask;
    Task *CmdTask;
    Task *EnbTask;
    Task *EncTask[MAX_NUM_MUSCLES];
    int activeMuscles[MAX_NUM_MUSCLES];
    int No_of_musc;
    double* LcData, CmdData, EncData;
    uInt32* EnbData;

public:
    Muscles();
    Muscles(int*,int);
    ~Muscles();
    void initMuscles();
    void startMuscles();
    //double* RW_Muscles(double*,uInt32);
    double* MuscleLc();
    double* MuscleEnc();
    void MuscleCmd(double*);
    void MuscleEnb(int);
    void MuscleEnb_(uInt32*);
    void stopMuscles();
    void deleteMuscles();
    void zeroMuscles();
    //same CmdHandle
    //array of CmdData
    //same LcHandle
    //array of LcData
    //same EnbHandle
    //0x of EnbData -> one bit for every motor
    //diff EncHandle
    //array of Enc Data
    //assists in creating a full muscle channel with 4 handles - Lc, Enb, CMd, Enc
};

class DAQ;

class Task
{
    typedef const char* string;

    TaskHandle task;
    string handleName;
    string slot;
    int type;
    int slotNo;
    int encNo;
    double* data;
    DAQ *daqObj;
    char* temp;
    //DAQ daqObj;
public:
    
    //void foo(DAQ* daqObj);
    Task();
    Task(void*&, string, string, int);
    //Task(TaskHandle,string,string,int);
    void assignTask(void*&,string,string,int,int enc=0);
    void assignTask(void*&,string,int,int);
    void createTask(void*&,string);
    void createChan();
    char* assignSlot(int);
    void initTask(int s=1); //after creating all the task only you should init the task
    void startTask();
    
    double* daqTask(void);
    void daqTask(double*);
    void daqTask(uInt32*);
    //template <typename T> void daqTask(T&);
    void stopTask();
    void deleteTask();
    void* getHandle(void);
    string getName(void);
    string getSlot(void);
    int getSlotNo(void);
    double* getData();
    int getType(void);
    ~Task();
    //DAQ* daqObj;
};

class DAQ
{
    typedef const char* string;

public:
    //void bar(Task& taskObj);
    DAQ(void);
    ~DAQ(void);
    void createHandle(void*&, string);
    void createChan(Task&);
    void createClk(Task&, int);
    void startHandle(void*&);
    void readDAQ(Task&);
    void writeDAQ(void*&, double*);
    void writeDAQ(void*&, uInt32*);
    void stopHandle(void*&);
    void clearHandle(void*&);
    void errorCheck(int, TaskHandle&);
};

#endif