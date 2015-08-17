/*
 * initFPGA.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: surajcha
 */
#include <iostream>
#include "pipeIO.h"
#include "okFrontPanelDLL.h"
#include "utilities.h"
#include <malloc.h>
#include <initFPGA.h>

int checkDLL() {
	char dll_date[32], dll_time[32];
	if (FALSE == okFrontPanelDLL_LoadLib(NULL)) {
		printf("FATAL: okFrontPanel compiled library could not be loaded.\n");
		waitkey();
		return(-1);
	}
	else {
		okFrontPanelDLL_GetVersion(dll_date, dll_time);
		printf("FrontPanel DLL loaded.  Built: %s  %s\n", dll_date, dll_time);
	}
	return 0;
}

okCFrontPanel * initFPGA::devPointer(){
	return dev;
}

initFPGA::initFPGA(char *bitfile, char *serial) {

	//Create driver object
	dev = new okCFrontPanel;

	//Connect to device over serial
	if ( dev->OpenBySerial(std::string(serial)) != okCFrontPanel::NoError ) {
		printf("FATAL: Device could not be opened");
		waitkey();
		//closeFPGA(dev);
		exit(1);
	}
	else {
		//obtain device info and setup clocking PLL
		dev->GetDeviceInfo(&m_devInfo);
		dev->LoadDefaultPLLConfiguration(); //Device PLL configuration

		// Print some general information about the XEM.
		printf("Device detected: %s\n", m_devInfo.productName);
		printf("Device firmware version: %d.%d\n", m_devInfo.deviceMajorVersion, m_devInfo.deviceMinorVersion);
		printf("Device serial number: %s\n", m_devInfo.serialNumber);
		printf("Device device ID: %d \n\t=>Device PLL has now been configured...\n", m_devInfo.productID);

		// Download the bitfile file to target FPGA
		if ( dev->ConfigureFPGA(bitfile) != okCFrontPanel::NoError ) {
			printf("FPGA configuration failed.\n");
			waitkey();
			//closeFPGA(dev);
			exit(1);
		}

		// Check for FrontPanel support in the FPGA configuration.
		if (dev->IsFrontPanelEnabled()){
			printf("\nFrontPanel support is enabled on Target Device.\n");
		}
		else{
			printf("FrontPanel support is not enabled on FPGA(Serial: %s) on port %s\n", m_devInfo.serialNumber, serial);
			waitkey();
			exit(1);
		}

		//Device initialized!
		printf("\n\nInitialization complete. FPGA(Serial: %s) on port %s is ready for use.\n", m_devInfo.serialNumber, serial);
	}
}

initFPGA::~initFPGA() {
	delete dev;
	//return 1;
}

