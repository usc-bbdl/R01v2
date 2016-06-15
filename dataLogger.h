#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

class dataLogger
{
    int samplingFrequency;//in Hertz
    TaskHandle  encodertaskHandle[2];
    TaskHandle  loadCelltaskHandle;
    int  totalSampleRead;
    FILE *dataFile;
public:
    dataLogger();
    ~dataLogger(void);
    //double muscleOptimalLength1,muscleOptimalLength2;
    double loadCellOffset1, loadCellOffset2;
    char *fileName;
    int trialLength;
    bool isRecording;
    int startRecording(void);
    int stopRecording(void);
    int reset(void);
};
#endif
