#include "SomeFpga.h"
#include "utilities.h"
#include <assert.h>
#include <Windows.h>
fpgaIONeuromorphic::fpgaIONeuromorphic(int fpgaType, int muscleType){
	this->fpgaType = fpgaType;
	this->muscleType = muscleType;
	this->forceAdressWire = 0x0032;
	this->forceAdressPipe = 0x0080;
	if ((fpgaType == SPINDLE_FPGA) && (muscleType == BICEP))
		myFPGA = fpgaIO(spindleBicFPGASerialNum,spindleBicFPGABitFile);
	fpgaIO::fpgaIO(std::string serialNum, std::string bitfile, /*uint16_t designHASH,*/ unsigned int confType) {
	else if ((fpgaType == SPINDLE_FPGA) && (muscleType == TRICEP))
		myFPGA = fpgaIO(spindleTriFPGASerialNum,spindleTriFPGABitFile);
	else if ((fpgaType == MOTOR_FPGA) && (muscleType == BICEP))
		myFPGA = fpgaIO(motorBicFPGASerialNum,motorBicFPGABitFile);
	else if ((fpgaType == MOTOR_FPGA) && (muscleType == TRICEP))
		myFPGA = fpgaIO(motorTriFPGASerialNum,motorTriFPGABitFile);
	else if ((fpgaType == CORTEX_FPGA) && (muscleType == BICEP))
		myFPGA = fpgaIO(cortexBicFPGASerialNum,cortexBicFPGABitFile);
	else if ((fpgaType == CORTEX_FPGA) && (muscleType == TRICEP))
		myFPGA = fpgaIO(cortexTriFPGASerialNum,cortexTriFPGABitFile);
}
int fpgaIONeuromorphic::readForceStream(float * muscleForceFpgaPipe){

}
int fpgaIONeuromorphic::readForceWire(float * muscleForceFpgaWire){
	uint32_t outVal[2];
	myFPGA->readData (int *forceAdressWire, 2, *outVal);
	int outValLo = outVal[0] & 0xffff;
	int outValHi = outVal[1] & 0xffff;
    int outValInt = ((outValHi << 16) + outValLo) & 0xFFFFFFFF;
    ReInterpret(outValInt, muscleForceFpgaWire);
    return 0;
}
	
}
//muscleFPGA->readForceStream(&muscleForceFpgaPipe);