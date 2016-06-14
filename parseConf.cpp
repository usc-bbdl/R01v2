#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <file.h>
#include "listman.h"
#include <string.h>

parseConf::parseConf(char *configLocation) {
  strcpy(confLocation, configLocation);
  getConf();
  channelParams = (channelParameters *)malloc(nChannels*sizeof(channelParameters));
}

~parseConf() {
  free(channelParams);
}

int numChannels() {
  return nChannels
}

channelParameters* readParameters(long int channelNumber) {
    return &(channelParams[channelNumber]);
}
int parseText() {
  //use REGEX to parse a word at a time
  return 1;
}

int getConf();
int getTest();
int getTrajectories();
