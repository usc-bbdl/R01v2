
#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include "listman.h"

class logger {
private:
  enum {
    CHAR,
    INT,
    FLOAT,
    DOUBLE
  } TYPE;

  // //REDO THIS LIST TO USE LISTMAN.H
  // typedef struct listElement{
  //   void *data;
  //   listElement *prev, *next;
  // } listElement;
  // int dataCount; //number of data points to log
  // listElement *listHead;

public:
  logger(); //initialize looger's basic variables, pointers to NULL
  ~logger(); //destroy all dynamic variables here
  int addElement(void *, TYPE, char *); //add variable pointed to by parameter to list
  int removeElement(listElement *);
  int getList(); //get list of variables to read from config file
};

#endif
