#ifndef ACQ_H
#define ACQ_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

class ACQ {
private:
  class ACQdata{

  };
public:
  ACQ(); //start NIDAQmx task for acquisition - encoder and loadcell cards to be attached to this task
  ~ACQ(); //stop and clear task, remove any dynamic variables
  int getData(void *);//gets data from cards and puts data in class pointed to by the void * parameter

};

#endif
