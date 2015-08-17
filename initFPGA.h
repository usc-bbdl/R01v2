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
	bool           m_bCheck=0;
	bool           m_bInjectError = 0;
	int            m_ePattern = 0;
	UINT32         m_u32BlockSize = 0;
	UINT32         m_u32SegmentSize = 0;
	UINT32         m_u32TransferSize = 0;
	UINT32         m_u32ThrottleIn = 0;
	UINT32         m_u32ThrottleOut = 0;
	clock_t        m_cStart = 0;
	clock_t        m_cStop = 0;
public:
	initFPGA(char *, char *);
	~initFPGA();
	okCFrontPanel * devPointer();
};

#endif /* INITFPGA_H_ */

