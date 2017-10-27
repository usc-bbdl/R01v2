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
static char* Lc_slot = "PXI1Slot5/"; // the physical Slot that has Anaog Output
static char* Cmd_slot = "PXI1Slot2/"; // the physical Slot that has Analog Input
static char* Enb_slot ="PXI1Slot2/port0"; // the physical Slot that has Digital Input
static char* Enc_slot ="PXI1Slot3/"; // the physical Slot that has Digital Output
static char* SampleClk = "/PXI1Slot5/ai/SampleClock"; //physical Slot that has the reference Clock

#define MAX_NUM_MUSCLES 7


// ports for each LoadCell, MotorOut and Encoder are defined in DAQ.cpp
// this class helps in getting that data
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
    TaskHandle LcHandle; // one TaskHandle for all the Lc tasks for all the Muscles
    TaskHandle CmdHandle; // one TaskHandle for all the Cmd tasks for all the Muscles
    TaskHandle EnbHandle; // one TaskHandle for all the Enb tasks for all the Muscles
    TaskHandle EncHandle[MAX_NUM_MUSCLES]; // individual TaskHandle for all the Enc tasks for all the Muscles
    Task *LcTask; // one Task object for the Lc Tasks
    Task *CmdTask; // one Task object for the Cmd Tasks
    Task *EnbTask; // one Task object for Enb Tasks
    Task *EncTask[MAX_NUM_MUSCLES]; // individual Task object for Enc Tasks
    int activeMuscles[MAX_NUM_MUSCLES]; // reference for all the muscles that are active
    int No_of_musc; 
	// data arguments for DAQ read and write are declared in Muscles class itself for a bottom-up approach
    double* LcData, CmdData, EncData; 
    uInt32* EnbData;

public:
    Muscles();
    Muscles(int*,int); //given the preferred muscle numbers and the No.of.Muscles, all the 4 Tasks for all the requested muscles are creates and assigned
    ~Muscles();
    void initMuscles(); // the assigned 4 tasks for al the Muscles are initialised
    void startMuscles(); // the initialised 4 tasks of all the Muscles are started
    //double* RW_Muscles(double*,uInt32);
    double* MuscleLc();  // the Lc Task object is called to read data. Since all the muscles are already associated with this Task object while they were initialized in Muscles(int*,int) just Task->daqTask() is called
    double* MuscleEnc(); // Its implementation is different than Lc even though both are Read tasks, since each encoder of the Muscle has distinct TaskHandle. but the function loops through all the encoders and returns the data in a similar format
    void MuscleCmd(double*); // the rotation for the Muscle motors are given and Task->dawTask() writes it
    void MuscleEnb(int); // arg 1 enables all the motors and 0 disables all the motors
    void MuscleEnb_(uInt32*); // enables only the motors encoded in the argument
    void stopMuscles(); // the running 4 tasks of all the Muscles are started
    void deleteMuscles(); // the 4 tasks of all the Muscles are started
    void zeroMuscles(); // all the initialised muscles are given a 0 rotation command
};

class DAQ;

class Task
{
    typedef const char* string; //local type cast

    TaskHandle task; // variable for storing the TaskHandle for the given Task
    string handleName; // a reference name for the TaskHandle
    string slot; // the corresponding slot for the given handle depending on its type
    int type; //type of the handle <Lc,Enc,Enb,Cmd>
    int slotNo; // the nth task of the same time, to get the corresponding port number
    int encNo;
    double* data; // depending on the type of Reading task <Lc or Enc> the size if dynamically assigned
    DAQ *daqObj; // DAQ class object for calling DAQ_class functions
    char* temp; // used to getting the concatenated Slots with port number

public:
    
    Task();
    Task(void*&, string, string, int);
    void assignTask(void*&,string,string,int,int enc=0); //was created for debugging purposes; its additional enc features are handled by the bellow implementation
    void assignTask(void*&,string,int,int); // creates the first instance of the given TaskHandle and calls createTask and createChan

    void createTask(void*&,string); //assigns the Handle to its Task by calling the DAQ -> createHandle
    void createChan(); // calls DAQ->createChan for assigning slot and port to the TaskHandle
    char* assignSlot(int); // returns the slot of the nth task of the same time, with the port numbers
    void initTask(int s=1); //after creating all the task only you should init the task
    void startTask();
    
    double* daqTask(void); //read data from DAQ
    void daqTask(double*); // write motorCmd data to DAQ
    void daqTask(uInt32*); // write motorEnb data to DAQ

    void stopTask(); 
    void deleteTask();
    void* getHandle(void);
    string getName(void);
    string getSlot(void);
    int getSlotNo(void);
    double* getData();
    int getType(void);
    ~Task();
};

class DAQ
{
    typedef const char* string; //local typecast for string from char*

public:
    DAQ(void); 
    ~DAQ(void);

    void createHandle(void*&, string); //a DAQ task is create for a given TaskHandle and a corresponding reference string
    void startHandle(void*&); //a DAQ task for the given TaskHandle is started
    void stopHandle(void*&); //a DAQ task for the given TaskHandle is stopped
    void clearHandle(void*&); //a DAQ task for the given TaskHandle is cleared

    void createChan(Task&); //a channel for any type of Task is created. Depending on the type of the task <Lc,Cmd,Enc,Enb> the function handles whether to create Anaog I/O or Digital I/O channels; The type determine which slot is to be assigned
    void createClk(Task&, int); // configure clocks for Tasks depending on its type
    
    void readDAQ(Task&); //reads Lc or Enc tasks
    void writeDAQ(void*&, double*); // write Cmd task
    void writeDAQ(void*&, uInt32*); // write Enb Task
    
    void errorCheck(int, TaskHandle&); // checks everytime a DAQ function is executed where the task failed or not. If failed, stops and clears the handle
};

#endif