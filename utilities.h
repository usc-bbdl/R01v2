#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H
#include <math.h>
#include <NIDAQmx.h>
#include <stdio.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

const int controlFreq = 1000;
const double PI = 3.14159265358979323846;
const double shaftRadius = 0.003;// motor shaft radius in cm
const double loadCellScale1 = (1/sqrt(2.0)) * 50.53; //From calibration test with weights
const double loadCellScale2 = (1/sqrt(2.0)) * 50.53; //From calibration test with weights
const int sampleFreq = 50000;
const int motorMinVoltage = -7;
const int motorMaxVoltage = 7;
const int messageMinVoltage = 0;
const double messageMaxVoltage = 0.03;
const int loadCellMinVoltage = -10;
const int loadCellMaxVoltage = +10;
const int encoderPulsesPerRev = 500;

/*
const uInt32     MESSAGE_NO_CONNECTION = 0x00000000;
const uInt32     MESSAGE_IDLE = 0x00000003;
const uInt32     MESSAGE_KINEMATIC = 0x00000004;
const uInt32     MESSAGE_RECORD = 0x00000005;
const uInt32     MESSAGE_PERTURB = 0x00000006;
const uInt32     MESSAGE_TERMINATE = 0x00000007;
*/

#define     STATE_INIT 0
#define     STATE_WINDING_UP 1
#define     STATE_OPEN_LOOP 2
#define     STATE_CLOSED_LOOP 3
#define     STATE_PARADIGM_LENGTH_CALIBRATION 4
#define     STATE_RUN_PARADIGM_SERVO_PERTURBATION 5
#define     STATE_RUN_PARADIGM_MANUAL_PERTURBATION 6
#define     STATE_RUN_PARADIGM_VOLUNTARY_MOVEMENT 7
#define     STATE_SHUTTING_DOWN 8

extern bool dataAcquisitionFlag[12];
const int   NUM_NEURON = 128;
const int   SAMPLING_RATE = 1024;
const int   DATA_EVT_LCEVEL = 9;
const int   DATA_EVT_SPINDLE_IA_GAIN = 1;
const int   DATA_EVT_SPINDLE_IA_OFFSET = 3;
const int   DATA_EVT_GAMMA_DYN = 4;
const int   DATA_EVT_GAMMA_STA = 5;
const int   DATA_EVT_SPINDLE_II_GAIN = 10;
const int   DATA_EVT_SPINDLE_II_OFFSET = 6;
const int   DATA_EVT_SYN_IA_GAIN = 3;
const int   DATA_EVT_SYN_CN_GAIN = 10;
const int   DATA_EVT_SYN_II_GAIN = 11;
const int   DATA_EVT_CORTEX_DRIVE = 8;
const int   DATA_EVT_CORTEX_MIXED_INPUT = 9;
const int   DATA_EVT_S_WEIGHT = 6;

int proceedState(int *);
int ReInterpret(float32, int32 *);
int ReInterpret(int32, int32 *);
int ReInterpret(int, float *);

typedef unsigned char       BYTE;
#define  BICEP 0
#define TRICEP 1
extern float GGAIN; //default is (0.9/1000) //0.4/2000 is safe
extern float TBIAS;
//work in progress
void configParser(FILE *);
void testParser(FILE *);

#endif

