#include "SomeFpga.h"
#include "utilities.h"
#include <assert.h>
#include <Windows.h>
fpgaIONeuromorphic::fpgaIONeuromorphic(int fpgaType, int muscleType){
	this->fpgaType = fpgaType;
	this->muscleType = muscleType;
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
fpgaIONeuromorphic::readForceStream(*float muscleForceFpgaPipe){

}
fpgaIONeuromorphic::readForceWire(*float muscleForceFpgaWire){
	
}

//muscleFPGA  = new fpgaIONeuromorphic(MOTOR_FPGA, BICEP);
//muscleFPGA->readForceStream(&muscleForceFpgaPipe);
//muscleFPGA->readForceWire(&muscleForceFpgaPipe);