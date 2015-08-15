#ifndef UTILITIES_H
#define UTILITIES_H
#include <math.h>


#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

const int controlFreq = 1000;
const double PI = 3.14159265358979323846;
const double shaftRadius = 0.003;// motor shaft radius in cm
const double loadCellScale1 = (1/sqrt(2.0)) * 6.1463; //From calibration test with weights
const double loadCellScale2 = (1/sqrt(2.0)) * 5.9382; //From calibration test with weights
const int sampleFreq = 50000;
const int motorMinVoltage = 0;
const int motorMaxVoltage = 9;
const int loadCellMinVoltage = -10;
const int loadCellMaxVoltage = +10;


#define     MOTOR_STATE_INIT 0
#define     MOTOR_STATE_WINDING_UP 1
#define     MOTOR_STATE_OPEN_LOOP 2
#define     MOTOR_STATE_CLOSED_LOOP 3
#define     MOTOR_STATE_RUN_PARADIGM 4
#define     MOTOR_STATE_SHUTTING_DOWN 5

int proceedState(int *);
#endif
