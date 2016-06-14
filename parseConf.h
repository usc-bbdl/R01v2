#ifndef PARSECONF_H
#define PARSECONF_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <file.h>
#include "listman.h"

typedef struct {
  float initpos, finalpos, jtvel;
  float jt1Ang, jt1MLn, jt2Ang, jt2MLn, Fbias, Fgain;
  int nChannels, trajbuffer;
  char tstfile[100], traject[100], logfile[100];
}exptParameters;

typedef struct {
  float LDcellgain, PIDgain, shaftdia;
  float gd, gs, alpha;
  float MNIaGain, MNIaOfst, MNIIGain, MNIIOfst, MNthresh, MNpkfire, MNmnfire;
  float $CXIaGain, CXIaOfst, CXIIGain, CXIIOfst;
}channelParameters

class parseConf {
private:
  char confLocation[100];
  exptParameters exptParams;
  channelParameters *channelParams;

public:
  parseConf(char *configLocation);
  ~parseConf();
  int numChannels();
  channelParameters* readParameters(long int channelNumber);
  int parseText(char *);
  int getConf();
  int getTest();
  int getTrajectories();
};

#endif
