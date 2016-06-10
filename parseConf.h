#ifndef PARSECONF_H
#define PARSECONF_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

typedef class {
  float initpos, finalpos, jtvel;
  float jt1Ang, jt1MLn, jt2Ang, jt2MLn, Fbias, Fgain;
  int nChannels;
}exptParameters;

typedef class {
  float LDcellgain, PIDgain, shaftdia;
  float gd, gs, alpha;
  float MNIaGain, MNIaOfst, MNIIGain, MNIIOfst, MNthresh, MNpkfire, MNmnfire;
  float $CXIaGain, CXIaOfst, CXIIGain, CXIIOfst;
}channelParameters

class parseConf {
private:
  char confLocation[100];

public:
  parseConf(char *, chnl);
  ~parseConf();
  int readConf();
  int readTest();
};

#endif
