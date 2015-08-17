/*
 * pipeIO.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: surajcha
 */
#include <iostream>
#include "pipeIO.h"
#include "okFrontPanelDLL.h"
#include "utilities.h"
#include <malloc.h>
#include <string.h>

pipeIO::pipeIO(initFPGA *temp) {
	xem = temp;
	setAddr(0xA0, 0x80);
}

int pipeIO::readPipe(unsigned char *fromNF_padded) {
	if (0 == m_u32BlockSize) {
		return( ((okCFrontPanel::ErrorCode)(xem->dev->ReadFromPipeOut(inAddr, m_u32SegmentSize, fromNF_padded)) ) );
	} else {
		return( ((okCFrontPanel::ErrorCode)(xem->dev->ReadFromBlockPipeOut(inAddr, m_u32BlockSize, m_u32SegmentSize, fromNF_padded)) ) );
	}
	return 0;
}

int pipeIO::writePipe(unsigned char *toNF_padded) {
	if (0 == m_u32BlockSize) {
		return( ( (okCFrontPanel::ErrorCode)(xem->dev->WriteToPipeIn(outAddr, m_u32SegmentSize, toNF_padded)) ) );
	} else {
		return( ( (okCFrontPanel::ErrorCode)(xem->dev->WriteToBlockPipeIn(outAddr, m_u32BlockSize, m_u32SegmentSize, toNF_padded)) ) );
	}
}

void pipeIO::setBlockSize(UINT32 size) {
	m_u32BlockSize = size;
	evalSizes();
}
void pipeIO::evalSizes() {
	if (0 != m_u32BlockSize) {
		if ( m_u32SegmentSize  != (m_u32SegmentSize % m_u32BlockSize) ) {
			printf("\nWARNING: Resizing Segment - Segment size must be a multiple of block length\n");
			m_u32SegmentSize  -= (m_u32SegmentSize % m_u32BlockSize); // Segment size must be a multiple of block length
		}
		if( m_u32TransferSize != (m_u32TransferSize % m_u32BlockSize) ) {
			printf("\nWARNING: Resizing Transfer Unit - Transfer size must be a multiple of block length\n");
			m_u32TransferSize -= (m_u32TransferSize % m_u32BlockSize);  // Segment size must be a multiple of block length
		}
	}
}

void pipeIO::statAddr(int *pIn, int *pOut) {
	*pIn = inAddr;
	*pOut = outAddr;
}

void pipeIO::setAddr(int pIN, int pOut) {
	inAddr = pIN;
	outAddr = pOut;
}

char * pipeIO::padData(char outNF[]) { //revisit to check for correctness
	int diff = 0, i = 0;
	//char filler[] = 0; //same as NULL
	if( strlen(outNF) < m_u32TransferSize) {
		for(diff = m_u32TransferSize - strlen(outNF); i < diff; i++) {
			strcat(outNF,NULL);
		}
	} else if(1) { //revisit here

	}
	return outNF;
}

//how we append would depend on definitions of Transfer Size and Segment Size. Revisit when you return

//selection between write and read must be based on trigger values. But how does one trigger a logic design that is inherently combinational.
	//this is creating unwanted registers in the Verilog - needs to be removed.
