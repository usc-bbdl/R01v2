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
    double loadCellData[7];
    double encoderData[7];
  };
public:
  //these functions are generic and are independent of DAQ drivers
  ACQ(); //start NIDAQmx task for acquisition - encoder and loadcell cards to be attached to this task
  ~ACQ(); //stop and clear task, remove any dynamic variables
  int getData(ACQdata *); //gets data from cards and puts data in class pointed to by the parameter

  //-------------------------//
  //Driver specific functions//
  //-------------------------//

  //NIDAQmx specific functions go here
  int NIstartTask(); //function starts task by calling NIDAQmx functions
  int NIstopTask(); //stop task
  int NIclearTask(); //clear task
  int NIgetData(ACQdata *); //acquire data from NI cards

  //Acromag AcPC482 specific functions here
  int AMgetData(ACQdata *); //acquire data from Acromag cards - leave empty for now
};

#endif
