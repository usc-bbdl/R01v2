#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H
#include <math.h>
#include <NIDAQmx.h>
#include <stdio.h>
//#include "DAQ.h"

#define TRUE  1
#define FALSE 0

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

// Muscle motor ID
#define SMARTA 0
#define SMARTB 1
#define FDS 2
#define PIP 3
#define DIP 4
#define LBR 5
#define EIP 6

#define toneForce   1 // force needed to maintain tone on muscle

#define ENCODER_COUNT 4
// MUSCLE COUNT
#define MUSCLE_NUM 7
#define NUM_JR3_CHANNELS 6
#define NUM_ANALOG_IN (MUSCLE_NUM + NUM_JR3_CHANNELS)
const int controlFreq = 1000;
const double PI = 3.14159265358979323846;

const double shaftRadius = 0.003;// motor shaft radius in cm

// Motor Load cell calibration
const double loadCellScale[7] = {   (1/sqrt(2.0)) * 57.815,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 77.069,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 59.105,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 56.236,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 61.450,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 62.350,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 58.827}; //From calibration test with weights
/*// OLD VALUES
const double loadCellScale[7] = {   (1/sqrt(2.0)) * 50.53,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 50.53,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 50.53,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 50.53,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 50.53,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 50.53,  //From calibration test with weights
                                    (1/sqrt(2.0)) * 50.53}; //From calibration test with weights
*/

const int       sampleFreq          = 50000;
const double    motorMinVoltage     = -7;
const double    motorMaxVoltage     = 7;
const int       messageMinVoltage   = 0;
const double    messageMaxVoltage   = 0.03;
const double    loadCellMinVoltage  = -10;
const double    loadCellMaxVoltage  = +10;
const int       encoderPulsesPerRev = 500;

// JR3 Calibration Matrix
const double calibrationMatrixJR3[6][6] =
{
    {13.669,0.229,0.105,-0.272,0.060,0.865},
    {0.160,13.237,-0.387,-0.027,-0.396,-0.477},
    {1.084,0.605,27.092,-2.88,-0.106,1.165},
    {-.007,-.003,-0.001,0.676,-0.002,-0.038},
    {0.004,-0.004,0.001,0.000,0.688,-0.012},
    {0.004,0.003,0.003,-0.006,0.013,0.665}
};

const double identityMatrixJR3[6][6] =
{
    {1.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,1.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,1.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,1.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,1.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,1.0}
};



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
#define     STATE_RUN_PARADIGM_CDMRP_IMPLANT 9

extern int dataAcquisitionFlag[12];
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
