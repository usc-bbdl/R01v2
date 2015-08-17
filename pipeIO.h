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
	okCFrontPanel *dev = NULL;
public:
	pipeIO();
	int readPipe(char *);
	int writePipe(char *);
	int statPipe();
};

#endif /* PIPEIO_H_ */
