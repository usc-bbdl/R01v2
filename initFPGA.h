/*
 * initFPGA.h
 *
 *  Created on: Aug 16, 2015
 *      Author: surajcha
 */
//NOTES ON EXECTUTION: Always run checkDLL at the beginning of the program to ensure okFrontPanel libraries are available
#ifndef INITFPGA_H_
#define INITFPGA_H_

#include <iostream>
#include "pipeIO.h"
#include "okFrontPanelDLL.h"
#include <malloc.h>
typedef unsigned int UINT32;

int checkDLL();

class initFPGA {
	//char bitfile[200], serial[100];
	okCFrontPanel *dev =NULL;

	//device specific parameters
	okTDeviceInfo  m_devInfo;

public:
	initFPGA(char *, char *);
	~initFPGA();
	okCFrontPanel * devPointer();
};

#endif /* INITFPGA_H_ */

