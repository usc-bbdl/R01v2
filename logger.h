
#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include "listman.h"
#include "motorData.h"

//class timeNprotocol{
    /*
public:
    double tick;
    double tock;
    int expProtocol;
    logger* myLogger;
    void nonsense();
};*/

class logger {
private:
  enum TYPE {
    CHAR,
    INT,
    FLOAT,
    DOUBLE
  } ;

  // //REDO THIS LIST TO USE LISTMAN.H
  // typedef struct listElement{
  //   void *data;
  //   listElement *prev, *next;
  // } listElement;
  // int dataCount; //number of data points to log
  // listElement *listHead;

  motorData* mData;

  FILE *dataFile;

  char header[200];

  double tick;
  double tock;
  int expProtocol;
public:
  logger(motorData* Data); //initialize looger's basic variables, pointers to NULL
  //todo
  //delete same content in motorcontrol

  ~logger(); //destroy all dynamic variables here

  void update(double tick, double tock, int expProtocol);

  //write 1 line of data to the file
  //create a thread for this
  void logOneLine();
  //todo 
  //replace file writing in controlloop with a thread of this

  int addElement(void *, TYPE, char *); //add variable pointed to by parameter to list
  int removeElement(listElement *);
  int getList(); //get list of variables to read from config file

  void startthread(void * parameters);
  static void writethread(void * parameters);

};

#endif
