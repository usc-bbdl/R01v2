/*
 * pipeIO.h
 *
 *  Created on: Aug 16, 2015
 *      Author: surajcha
 */

#ifndef PIPEIO_H_
#define PIPEIO_H_

#include "okFrontPanelDLL.h"
#include "utilities.h"

class pipeIO {
	initFPGA *xem = NULL;

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

	int inAddr, outAddr;
	void evalSizes();
public:
	pipeIO(initFPGA *);
	int readPipe(unsigned char *);
	int writePipe(unsigned char *);
	void setBlockSize(UINT32);
	void statAddr(int*, int*);
	void setAddr(int, int);
	char* padData(char []);
};

#endif /* PIPEIO_H_ */
