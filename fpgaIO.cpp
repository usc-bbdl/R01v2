#include <iostream>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
//#include <string.h>
//#include "./include/utilities.h"
#include "fpgaIO.h"
#ifdef _WIN64
        #include "../lib/FrontPanel/latest/nt64/okFrontPanelDLL.h"
#elif __linux__
        #include "../lib/FrontPanel/latest/ubuntu64/API/okFrontPanelDLL.h"
#endif

//*******************************
// Library function definitions *
//*******************************

//bit-level re-interpretation of data for USB 2.0 bus
int fpgaIOcheck() {
	char dll_date[32], dll_time[32];
    	if(FALSE == okFrontPanelDLL_LoadLib(NULL)){
        	std::cerr<<"FrontPanel library could not be loaded.\n";
		return 1;
	}
	okFrontPanelDLL_GetVersion(dll_date, dll_time);
    	std::cerr<<"FrontPanel DLL loaded.  Built: " << dll_date << dll_time << std::endl;
	return 0;
}

int errHandler(okCFrontPanel::ErrorCode retVal) {
	switch (retVal) {
		case ok_Failed:
				std::cerr<<"OK FrontPanel: FPGA operation failed!\n"; 
				break;
		case ok_Timeout:
				std::cerr<<"OK FrontPanel: FPGA operation timed out!\n"; 
				break;
		case ok_DoneNotHigh:
				std::cerr<<"OK FrontPanel: 'Done' signal not logic high!\n"; 
				break;
		case ok_TransferError:
				std::cerr<<"OK FrontPanel: FPGA data transfer failed!\n"; 
				break;
		case ok_CommunicationError:
				std::cerr<<"OK FrontPanel: FPGA communication error!\n"; 
				break;
		case ok_InvalidBitstream:
				std::cerr<<"OK FrontPanel: Invalid bitstream selected!\n"; 
				break;
		case ok_FileError:
				std::cerr<<"OK FrontPanel: File error!\n"; 
				break;
		case ok_DeviceNotOpen:
				std::cerr<<"OK FrontPanel: FPGA device does not open!\n"; 
				break;
		case ok_InvalidEndpoint:
				std::cerr<<"OK FrontPanel: FPGA endpoint invalid!\n"; 
				break;
		case ok_InvalidBlockSize:
				std::cerr<<"OK FrontPanel: Transfer block size is invalid!\n"; 
				break;
		case ok_I2CRestrictedAddress:
				std::cerr<<"OK FrontPanel: I2C address is restricted\n"; 
				break;
		case ok_I2CBitError:
				std::cerr<<"OK FrontPanel: Bit error on I2C bus\n"; 
				break;
		case ok_I2CNack:
				std::cerr<<"OK FrontPanel: NACK received on I2C bus\n"; 
				break;
		case ok_I2CUnknownStatus:
				std::cerr<<"OK FrontPanel: I2C device status unknown!\n"; 
				break;
		case ok_UnsupportedFeature:
				std::cerr<<"OK FrontPanel: Feature is not supported!\n"; 
				break;
		case ok_FIFOUnderflow:
				std::cerr<<"OK FrontPanel: Transfer FIFO underflow!\n"; 
				break;
		case ok_FIFOOverflow:
				std::cerr<<"OK FrontPanel: Transfer FIFO overflow!\n"; 
				break;
		case ok_DataAlignmentError:
				std::cerr<<"OK FrontPanel: Data alignment error in transfer!\n"; 
				break;
		case ok_InvalidResetProfile:
				std::cerr<<"OK FrontPanel: FPGA reset profile invalid!\n"; 
				break;
		case ok_InvalidParameter:
				std::cerr<<"OK FrontPanel: Invalid parameter(s)!\n"; 
				break;
		default:
			break;
	}
	return (int)retVal;
}

int errHandler(okCFrontPanel::ErrorCode retVal, std::string userMSG) {
	if( (userMSG.length() > 1) && ((int)retVal != (int)ok_NoError) ) {
		std::cerr<<std::endl<<"FPGA I/O: "<<userMSG<<"\n\t";
	}
	return errHandler(retVal);
}

int enumerateFPGA() {
	okCFrontPanel probeDev;
	int devCount = probeDev.GetDeviceCount();
	std::cout<<"FPGA Device Enumeration: "<<devCount<<" devices found!\n";
	int i;
	for (i = 0; i < devCount; i++) {
		std::cout << "Device " << probeDev.GetDeviceListModel(i);
		std::cout <<" - Serial: " << probeDev.GetDeviceListSerial(i) << "\n";
	}

	return devCount;
}


//*********************************************
// library class private function definitions *
//*********************************************
uint32_t fpgaIO::bitFormat(uint32_t data) {
	return (selectUSB == 2)? (data & 0xFFFF): data;
	//     test condition  ? lower 16 bits  : full 32 bits
}

int fpgaIO::fpgaInit(std::string serialID) {
	deviceReady = false;
	int retVal =  errHandler(dev.OpenBySerial(serialID), "Open FPGA device");
	if(retVal != 0) 
		return retVal;
	if((retVal=errHandler(dev.GetDeviceInfo(&deviceInfo), "FPGA Initialization")) != 0)
		return retVal;

	//test device interfaces - bus negotiation parameters
	if(deviceInfo.deviceInterface == OK_INTERFACE_USB3) {
		selectUSB = 3;
	} else if(deviceInfo.deviceInterface == OK_INTERFACE_USB2) {
		selectUSB = 2;
	} else if(deviceInfo.deviceInterface == OK_INTERFACE_PCIE) {
		std::cerr<<"FPGA Initialization: Device on PCI-E interface. BUT HOW?\n";
		selectUSB = 0;
	} else {
		std::cerr<<"FPGA Initialization: Device on unkown interface\n";
		std::cerr<<"                     -WARNING: I/O primitives disabled!\n";
		selectUSB = 0;
		deviceReady = false;
		return -15;
	}
	//check USB throughput
	usbSpeed = ((int)deviceInfo.usbSpeed);
	if(usbSpeed == 0) {
		std::cerr<<"FPGA Initialization: Unknown USB bus speed\n";
		std::cerr<<"                     -WARNING: I/O primitives disabled!\n";
		deviceReady = false;
		return -15;
	} else if( (usbSpeed < selectUSB) && (selectUSB == 2 || selectUSB == 3) ) {
		std::cerr<<"FPGA Initialization: WARNING: USB bus can perform faster!\n";
	}
	
	//check PLL type
	if(deviceInfo.isPLL22150Supported) {
		selectPLL = 150;
	} else if(deviceInfo.isPLL22393Supported) {
		selectPLL = 393;
	} else {
		std::cerr<<"FPGA Initialization: Unsupported PLL device\n";
		std::cerr<<"                     -WARNING - PLL reconfiguration disabled!\n";
		selectPLL = 0;
	}
	
	//Check if device firmware is FrontPanel 3+
		//support for block-throttled pipes
	if(dev.IsFrontPanel3Supported()) {
		isFP3 = true;
	} else {
		isFP3 = false;
	}
	deviceReady = true;
	return retVal;
}

int fpgaIO::fpgaBurn(std::string bitfile) {
	if(bitfile.length() <= 1) { 
		std::cout<<"FPGA design configuration: No bitfile to burn. Can't burn blank design\n";
		std::cout<<"                           -WARNING: I/O primitives disabled!\n";
		designBurned = false;
		deviceReady  = false;
		return -15;
	}
	return errHandler(dev.ConfigureFPGA(bitfile), "FPGA design configuration");
}

int fpgaIO::getDeviceInfo() {
	return errHandler(dev.GetDeviceInfo(&deviceInfo));
}


//*********************************************
// PUBLICALLY ACCESSIBLE FUNCTIONS BEGIN HERE *
//*********************************************

fpgaIO::fpgaIO(std::string serialNum, std::string bitfile, /*uint16_t designHASH,*/ unsigned int confType) {
	serialID = serialNum;
	bitFile  = bitfile;
	int retVal;
	//initialize device
	if((retVal = fpgaInit(serialID)) != 0) {
		std::cerr<<"FPGA Initialization: FATAL ERROR! Unable to open device\n";
		std::cerr<<"                     -WARNING: device unusable\n";
		//handle inactive FPGA here
		//
		//
		//
		} else {
			//setup clock generator PLLs for device
			if((retVal = loadClockGenfromEEPROM()) != 0) {
				std::cerr<<"FPGA Initialization: I2C clock generators not available\n";
				std::cerr<<"                     -WARNING: PLL routines are not supported\n";
				setPLL = false;
				selectPLL = 0;
			} else {
				setPLL = 1;			
				readClockGenSettings();
			}
			//if USB 3.0, set default FMC settings and sensor states
				//also link appropriate IO primitives
			if(selectUSB == 3) {
				setFMC = true;
				loadSettings(&listSettings);
				readSensorStates();
			} else if(selectUSB == 2) {
				setFMC = false;
			}
			
			//here we check if design has fp enabled - if no, auto conf and check again
			if(!dev.IsFrontPanelEnabled()) {
				if(confType == MANUALCONF) {
					std::cerr<<"FPGA Initialization: FrontPanel not enabled on design\n";
					std::cerr<<"                     -WARNING: I/O primitives disabled!\n";
					deviceReady = false;
					retVal = -15;
				} else {
					std::cerr<<"FPGA Initialization: FrontPanel not enabled on device\n";
					std::cerr<<"                     - Auto-burning selected FPGA bitfile\n";
					designBurned = false;
					if ((retVal = fpgaBurn(bitFile)) != 0) {
						std::cerr<<"FPGA Initialization: FATAL ERROR! Unable to burn design\n";
						std::cerr<<"                     -WARNING: device unusable\n";
					} else {
						designBurned = true;
					}
					if(!dev.IsFrontPanelEnabled()){
						std::cerr<<"FPGA Initialization: FrontPanel not enabled on design\n";
						std::cerr<<"                     -WARNING: I/O primitives disabled!\n";
						deviceReady = false;
						retVal = -15;
					}
				}
			} else {
				designBurned = true;	
			}
			/*---------------\\
			|| HASH CHECKING ||
			\\---------------*/
			//here check if FPGA HDL has same HASH code as desired value.
			//if HASH values match, then design is good and can be used. keep deviceBurned = 0 since we didn't burn the bitfile in this run.
			//if HASH is a mismatch, then reset and burn the bitfile again
				//if FPGA doesn't talk after successful BURN, it is a standalone design
					//make deviceReady = 0 and disallow I/O primitives.
		}
}// end of constructor

fpgaIO::~fpgaIO() {
	dev.Close();
}

bool fpgaIO::isDeviceReady() {
	return (deviceReady==1)? true: false;
}

bool fpgaIO::isDesignBurned() {
	return (designBurned==1)? true: false;
}

bool fpgaIO::isPreConfig() {
	return (deviceReady==1 && designBurned == 0)? true: false;
}

//Clock Generator routines
double fpgaIO::getReference() {
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		return pll393.GetReference();
	}
	return pll150.GetReference();
}

int fpgaIO::setReference(double refFreq) {
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		pll393.SetReference(refFreq);
		return 0;
	}
	pll150.SetReference(refFreq, true);
	return 0;
}

double fpgaIO::getPLLFreq(unsigned int PLLNum) {
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		if(PLLNum >= PLLRecruits) {
			std::cerr<<"Clock Generator Warning: Requested PLL Number > available PLLs. Falling back to first PLL";
			PLLNum = 0;
		}
		return pll393.GetPLLFrequency(PLLNum);
	}
	return pll150.GetVCOFrequency();
}

int fpgaIO::getPLLP(unsigned int PLLNum) {
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		if(PLLNum >= PLLRecruits) {
			std::cerr<<"Clock Generator Warning: Requested PLL Number > available PLLs. Falling back to first PLL";
			PLLNum = 0;
		}
		return pll393.GetPLLP(PLLNum);
	}
	return pll150.GetVCOP();
}

int fpgaIO::getPLLQ(unsigned int PLLNum) {
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		if(PLLNum >= PLLRecruits) {
			std::cerr<<"Clock Generator Warning: Requested PLL Number > available PLLs. Falling back to first PLL";
			PLLNum = 0;
		}
		return pll393.GetPLLQ(PLLNum);
	}
	return pll150.GetVCOQ();
}

bool fpgaIO::setPLLParameters(unsigned int PLLNum, int p, int q, bool enable) {
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		if(PLLNum >= PLLRecruits) {
			std::cerr<<"Clock Generator Warning: Requested PLL Number > available PLLs. Falling back to first PLL";
			PLLNum = 0;
		}
		return pll393.SetPLLParameters(PLLNum, p, q, enable);
	}
	return  pll150.SetVCOParameters(p, q);
}

bool fpgaIO::isPLLEnabled(unsigned int PLLNum) {
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		if(PLLNum >= PLLRecruits) {
			std::cerr<<"Clock Generator Warning: Requested PLL Number > available PLLs. Falling back to first PLL";
			PLLNum = 0;
		}
		return pll393.IsPLLEnabled(PLLNum);
	}
	std::cerr<<"Clock Generator Warning: PLL VCO always enabled"; 
	return true;
}

int fpgaIO::getDivider(unsigned int outNum) {
	if(outNum >= PLLOutCount) {
			std::cerr<<"Clock Generator Warning: Requested PLL output > available outputs. Falling back to first output";
			outNum = 0;
	}
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		return pll393.GetOutputDivider(outNum);
	}
	if (outNum == 0) {
		return pll150.GetDiv1Divider();
	}
	return pll150.GetDiv2Divider();
}

bool fpgaIO::setDivider(unsigned int outNum, int div) {
	if(outNum >= PLLOutCount) {
			std::cerr<<"Clock Generator Warning: Requested PLL output > available outputs. Falling back to first output";
			outNum = 0;
	}
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {		
		return pll393.SetOutputDivider(outNum, div);
	}
	if (outNum == 0) {
		pll150.SetDiv1(pll150.GetDiv1Source(), div);
		return 0;
	}
	pll150.SetDiv2(pll150.GetDiv2Source(), div);
	return 0;
}


int fpgaIO::getOutputSource(unsigned int outNum) {
	if(outNum >= PLLOutCount) {
			std::cerr<<"Clock Generator Warning: Requested PLL output > available outputs. Falling back to first output";
			outNum = 0;
	}
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		return ((int)(pll393.GetOutputSource(outNum)/2));
	}
	return ((int)(pll150.GetOutputSource(outNum)==0?0:(pll150.GetOutputSource(outNum)==4?2:1)));
}


bool fpgaIO::setOutputSource(unsigned int outNum) {
	if(outNum >= PLLOutCount) {
			std::cerr<<"Clock Generator Warning: Requested PLL output > available outputs. Falling back to first output";
			outNum = 0;
	}
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		return pll393.SetOutputSource(outNum, okCPLL22393::ClkSrc_PLL0_0);
	}
	pll150.SetOutputSource(outNum, ((outNum==0)?okCPLL22150::ClkSrc_Div1ByN:okCPLL22150::ClkSrc_Div2ByN));
	return 0;
}

bool fpgaIO::isOutputEnabled(unsigned int outNum) {
	if(outNum >= PLLOutCount) {
			std::cerr<<"Clock Generator Warning: Requested PLL output > available outputs. Falling back to first output";
			outNum = 0;
	}
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		return pll393.IsOutputEnabled(outNum);
	}
	return pll150.IsOutputEnabled(outNum);
}

bool fpgaIO::setOutputEnable(unsigned int outNum, bool enable) {
	if(outNum >= PLLOutCount) {
			std::cerr<<"Clock Generator Warning: Requested PLL output > available outputs. Falling back to first output";
			outNum = 0;
	}
	if(selectPLL == 0) {
		std::cerr<<"Clock Generator Error: Unsupported Feature\n";
		return -15;
	}else if(selectPLL == 393) {
		pll393.SetOutputEnable(outNum, enable);
		return 0;
	}
	pll150.SetOutputEnable(outNum, enable);
	return 0;
}


int fpgaIO::readClockGenSettings() {
	//here, read value from I2C and put in pll150/393
	if(selectPLL == 0) {
                std::cerr<<"Clock Generator Error: Unsupported Feature\n";
                return -15;
        }else if(selectPLL == 393) {
                return errHandler(dev.GetPLL22393Configuration(pll393), "PLL read from I2C");
        }
	return errHandler(dev.GetPLL22150Configuration(pll150), "PLL read from I2C");
}

int fpgaIO::writeClockGenSettings() {
	//here, write user defined values into pll150/393 and in turn write that to I2C
	if(selectPLL == 0) {
                std::cerr<<"Clock Generator Error: Unsupported Feature\n";
                return -15;
        }else if(selectPLL == 393) {
                return errHandler(dev.SetPLL22393Configuration(pll393), "PLL read from I2C");
        }
	return errHandler(dev.SetPLL22150Configuration(pll150), "PLL read from I2C");
}

int fpgaIO::readClockGenfromEEPROM() {
	if(selectPLL == 0) {
                std::cerr<<"Clock Generator Error: Unsupported Feature\n";
                return -15;
        }else if(selectPLL == 393) {	
		return errHandler(dev.GetEepromPLL22393Configuration(pll393), "PLL read from EEPROM");
        }
	return errHandler(dev.GetEepromPLL22150Configuration(pll150), "PLL read from EEPROM");
}

int fpgaIO::loadClockGenfromEEPROM() {
	if (selectPLL != 0) {
		return errHandler(dev.LoadDefaultPLLConfiguration(), "PLL load from EEPROM");
	}
	std::cerr<<"Clock Generator Error: Unsupported Feature\n";
	return -15;
}

//FMC settings and sensor state routines

int fpgaIO::readSettings(settingsList *list) {
	if(selectUSB == 3) {
		int retVal = errHandler(dev.GetDeviceSettings(settings), "Get FMC Settings");
		if(retVal == 0) {
			settings.GetInt("FMC1_VADJ_VOLTAGE", &(list->FMCVoltageLevel));
			settings.GetInt("FMC1_VADJ_ENABLE" , &(list->FMCVoltageEnable));
			settings.GetInt("FMC1_VADJ_MODE"   , &(list->FMCVoltageMode));
		}
		return retVal;
	}
	std::cerr<<"Get FMC Settings: Device does not have USB 3.0 support\n";
	return -15;
}

int fpgaIO::loadSettings(settingsList *list) {
	if(selectUSB == 3) {
		settings.SetInt("FMC1_VADJ_VOLTAGE", 330);
		settings.SetInt("FMC1_VADJ_ENABLE" , 1);
		settings.SetInt("FMC1_VADJ_MODE"   , 3);
		int retVal = readSettings(&listSettings);
		if(retVal == 0) {
			settings.Save();
		}
		return retVal;
	}
	std::cerr<<"Set FMC Settings: Device does not have USB 3.0 support\n";
	return -15;
}

int fpgaIO::readSensorStates() {
	if (selectUSB == 3) {
		// Populate DeviceSensors
		int retVal = errHandler(dev.GetDeviceSensors(sensors), "Get Sensor States");
	 	if (retVal != 0) {
			return retVal;
		}
		// Retrieve individual sensors using an index value and the GetSensor method
		int i;
		for (i = 0; i < sensors.GetSensorCount(); i++){
			sensor = sensors.GetSensor(i);
			std::cout << sensor.name << "\n";
			std::cout << sensor.value << "\n";
		}
		return retVal;
	}
	std::cerr<<"Get Sensor States: Device does not have USB 3.0 support\n";
	return -15;
}

// I/O P1rimitives
	//wire based I/O
int fpgaIO::readData(int *epAddr, int length, uint32_t *outVal) {
	if (deviceReady != 1) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	int i;
	/*
	int retVal;
	if((retVal = errHandler(dev.UpdateWireOuts())) == 0) {
		for(i = 0; i < length; i++) {
			outVal[i] = bitFormat(dev.GetWireOutValue(epAddr[i]));
		}
		return 0;
	}
	return retVal;
	*/
	for(i = 0; i < length; i++) {
		outVal[i] = bitFormat(dev.GetWireOutValue(epAddr[i]));
	}
	return 0;
}

int fpgaIO::writeData(int *epAddr, int length, uint32_t *inVal, uint32_t *mask) {
	if (deviceReady != 1) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	int retVal, i;
	for(i = 0; i < length; i++) {
		if( (retVal = errHandler(dev.SetWireInValue(epAddr[i], bitFormat(inVal[i]), bitFormat(mask[i]) ))) != 0) {
			return retVal;
		}
	}
	//return  errHandler(dev.UpdateWireIns());
	return 0;
}

	//trigger based I/O

int fpgaIO::getTrigger(int *epAddr, int length, bool *outVal, uint32_t *mask) {
	if (deviceReady != 1) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	int i;
	/*
	int retVal;
	if( (retVal = errHandler(dev.UpdateTriggerOuts())) == 0) {
		for(i = 0; i < length; i++) {
			outVal[i] = dev.IsTriggered(epAddr[i], bitFormat(mask[i]));
		}
		return 0;
	}
	return retVal;
	*/
	for(i = 0; i < length; i++) {
		outVal[i] = dev.IsTriggered(epAddr[i], bitFormat(mask[i]));
	}
	return 0;
}

int fpgaIO::setTrigger(int epAddr, uint32_t bitVal) {
	if (deviceReady != 1) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	return errHandler(dev.ActivateTriggerIn(epAddr, bitFormat(bitVal) ));
}

	//pipes bases I/O
int fpgaIO::readStream(int epAddr, long length, unsigned char *data) {
	if (deviceReady != 1) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	//CRS
	return errHandler((okCFrontPanel::ErrorCode)dev.ReadFromPipeOut(epAddr, length, data));
}

int fpgaIO::writeStream(int epAddr, long length, unsigned char *data) {
	if (deviceReady != 1) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	return errHandler((okCFrontPanel::ErrorCode)dev.WriteToPipeIn(epAddr, length, data));
}

	//block-throttled pipes based I/O
int fpgaIO::readBlock(int epAddr, int blockSize, long length, unsigned char *data) {
	if ( !(deviceReady == 1 && isFP3 == 1) ) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	return errHandler((okCFrontPanel::ErrorCode)dev.ReadFromBlockPipeOut(epAddr, blockSize, length, data) );
}

int fpgaIO::writeBlock(int epAddr, int blockSize, long length, unsigned char *data) {
	if ( !(deviceReady == 1 && isFP3 == 1) ) {
		std::cerr<<"FPGA I/O: Unsupported feature\n";
		return -15;
	}
	return (errHandler((okCFrontPanel::ErrorCode)dev.WriteToBlockPipeIn(epAddr, blockSize, length, data) ));
}
