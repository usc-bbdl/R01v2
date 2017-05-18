#include "SomeFpga.h"
#include "utilities.h"
#include <assert.h>
#include <Windows.h>
#include <fpgaIONeuromorphic.h>
#include <fpgaIO.h>
const std::string spindleBicFPGASerialNum = "113700021E";
const std::string motorBicFPGASerialNum = "0000000542";
const std::string cortexBicFPGASerialNum = "0000000547";
const std::string spindleTriFPGASerialNum = "11160001CG";
const std::string motorTriFPGASerialNum = "1137000222";
const std::string cortexTriFPGASerialNum = "000000054B";


const std::string spindleBicFPGABitFile= "spindle.v";
const std::string motorBicFPGABitFile = "muscle.v";
const std::string cortexBicFPGABitFile = "cortex.v";
const std::string spindleTriFPGABitFile = "spindle.v";
const std::string motorTriFPGABitFile = "muscle.v";
const std::string cortexTriFPGABitFile = "cortex.v";

fpgaIONeuromorphic::fpgaIONeuromorphic(int fpgaType, int muscleType){
	this->fpgaType = fpgaType;
	this->muscleType = muscleType;
	this->forceAdressWire = 0x0032;
	this->forceAdressPipe = 0x0080;
	this->forceAdressBlock = 0x00BF;
	if ((fpgaType == SPINDLE) && (muscleType == BICEP))
		fpgaIO myFPGA(spindleBicFPGASerialNum,spindleBicFPGABitFile,MANUALCONF);
	else if ((fpgaType == SPINDLE) && (muscleType == TRICEPS))
		fpgaIO myFPGA(spindleTriFPGASerialNum,spindleTriFPGABitFile,MANUALCONF);
	else if ((fpgaType == MOTOR) && (muscleType == BICEP))
		fpgaIO myFPGA(motorBicFPGASerialNum,motorBicFPGABitFile,MANUALCONF);
	else if ((fpgaType == MOTOR) && (muscleType == TRICEPS))
		fpgaIO myFPGA(motorTriFPGASerialNum,motorTriFPGABitFile,MANUALCONF);
	else if ((fpgaType == CORTEX) && (muscleType == BICEP))
		fpgaIO myFPGA(cortexBicFPGASerialNum,cortexBicFPGABitFile,MANUALCONF);
	else if ((fpgaType == CORTEX) && (muscleType == TRICEPS))
		fpgaIO myFPGA(cortexTriFPGASerialNum,cortexTriFPGABitFile,MANUALCONF);
}

/**int fpgaIONeuromorphic::readMuscleSignals(){
	int blockSize = 2;
	long length = 128; //128 bits is 16 bytes
	unsigned char blockData [128];
	readBlock(forceAdressBlock, blockSize, length, unsigned char *blockData);
	int outValLo = *(((int *) blockData) +  8) + & 0xffff;
    int outValLo = *(((int *) blockData) + 10) + & 0xffff;
    int outValInt = ((outValHi << 16) + outValLo) & 0xFFFFFFFF;
	ReInterpret(outValInt, &(muscleForcePipe));
}
int fpgaIONeuromorphic::readForceWire(){
	uint32_t outVal[2];
	float force;
	myFPGA->readData (int *forceAdressWire, 2, *outVal);
	int outValLo = outVal[0] & 0xffff;
	int outValHi = outVal[1] & 0xffff;
    int outValInt = ((outValHi << 16) + outValLo) & 0xFFFFFFFF;
    ReInterpret(outValInt, &(muscleForceWire));
    return 0;
}
**/	

//muscleFPGA->readForceStream(&muscleForceFpgaPipe);