#ifndef ACT_H
#define ACT_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

class ACT {
private:
  class ACTdata{
    double loadCellData[7];
    double encoderData[7];
  };
public:
  //these functions are generic and are independent of DAQ drivers
  ACT(); //start NIDAQmx task for acquisition - Analog Our cards to be attached to this task
  ~ACT(); //stop and clear task, remove any dynamic variables
  int setData(ACTdata *); //sets data from cards and puts data in class pointed to by the parameter

  //-------------------------//
  //Driver specific functions//
  //-------------------------//

  //NIDAQmx specific functions go here
  int NIstartTask(); //function starts task by calling NIDAQmx functions
  int NIstopTask(); //stop task
  int NIclearTask(); //clear task
  int NIsetData(ACTdata *); //set data from NI cards

  //Acromag AcPC482 specific functions here
  int AMsetData(ACTdata *); //set data from Acromag cards - leave empty for now
};

#endif
