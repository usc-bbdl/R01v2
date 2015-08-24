#include "SomeFpga.h"
#include "utilities.h"
#include <assert.h>
#include <Windows.h>
#include "okFrontPanelDLL.h"

SomeFpga::SomeFpga(int NUM_NEURON = 128, int SAMPLING_RATE = 1024, std::string serX = "")
{
    this->NUM_NEURON = NUM_NEURON;
    this->SAMPLING_RATE = SAMPLING_RATE;
    
    this->xem = new okCFrontPanel;

    std::cout << "Connecting to OpaKelly of serial number: " << serX << std::endl;
    //std::cout<<std::endl<<"Error Code"<<(this->xem->OpenBySerial(serX));
    this->xem->OpenBySerial(serX);
    assert(this->xem->IsOpen());
//this->xem->LoadDefaultPLLConfiguration();

}

SomeFpga::~SomeFpga()
{
    delete this->xem;
}


int SomeFpga::ReadFpga(BYTE getAddr, char *type, float *outVal)
{
    xem -> UpdateWireOuts();
    // Read 18-bit integer from FPGA
    if (0 == strcmp(type, "int32"))
    {
       
    }
    // Read 32-bit signed integer from FPGA
    else if (0 == strcmp(type, "float32"))
    {
        int outValLo = xem -> GetWireOutValue(getAddr) & 0xffff;
        int outValHi = xem -> GetWireOutValue(getAddr + 0x01) & 0xffff;
        int outValInt = ((outValHi << 16) + outValLo) & 0xFFFFFFFF;
        ReInterpret(outValInt, outVal);
    }

    return 0;
}


int SomeFpga::ReadFpga(BYTE getAddr, char *type, int *outVal)
{
    xem -> UpdateWireOuts();
    // Read 18-bit integer from FPGA
    if (0 == strcmp(type, "int32"))
    {
        int32 outValLo = xem -> GetWireOutValue(getAddr) & 0xffff;
        int32 outValHi = xem -> GetWireOutValue(getAddr + 0x01) & 0xffff;
        int32 outValInt = ((outValHi << 16) + outValLo) & 0xFFFFFFFF;
        *outVal = (int) outValInt;
            //memcpy(outVal, &outValInt, sizeof(*outVal));
        //outVal = ConvertType(outVal, 'I', 'f')
        //#print outVal
    }
    // Read 32-bit signed integer from FPGA
    else if (0 == strcmp(type, "float32"))
    {

    }

    return 0;
}


int SomeFpga::SendPara(int bitVal, int trigEvent)
{
    int bitValLo = bitVal & 0xffff;
    int bitValHi = (bitVal >> 16) & 0xffff;
    this->xem->SetWireInValue(0x01, bitValLo, 0xffff);
    this->xem->SetWireInValue(0x02, bitValHi, 0xffff);
    this->xem->UpdateWireIns();            
    this->xem->ActivateTriggerIn(0x50, trigEvent)  ;
    return 0;
}





int SomeFpga::WriteFpgaLceVel(int32 bitValLce, int32 bitValVel, int32 bitValM1Voluntary, int32 bitValM1Dystonia, int32 trigEvent)
{
    //bitVal = 0;
    int32 bitValLo = bitValLce & 0xffff;
    int32 bitValHi = (bitValLce >> 16) & 0xffff;

    
    //Send muscle lce to fpga
    if (okCFrontPanel::NoError != this->xem->SetWireInValue(0x01, bitValLo, 0xffff)) {
		printf("SetWireInLo failed.\n");
		//delete xem;
		return -1;
	}
    if (okCFrontPanel::NoError != this->xem -> SetWireInValue(0x02, bitValHi, 0xffff)) {
		printf("SetWireInHi failed.\n");
		//delete xem;
		return -1;
	}


    bitValLo = bitValVel & 0xffff;
    bitValHi = (bitValVel >> 16) & 0xffff;    
    //send muscle velocity to Fpga
    if (okCFrontPanel::NoError != this->xem -> SetWireInValue(0x03, bitValLo, 0xffff)) {
		printf("SetWireInLo failed.\n");
		//delete xem;
		return -1;
	}
    if (okCFrontPanel::NoError != this-> xem -> SetWireInValue(0x04, bitValHi, 0xffff)) {
		printf("SetWireInHi failed.\n");
		//delete xem;
		return -1;
	}

    

    bitValLo = bitValM1Voluntary & 0xffff;
    bitValHi = (bitValM1Voluntary >> 16) & 0xffff;    
    //send muscle velocity to Fpga
    if (okCFrontPanel::NoError != this->xem -> SetWireInValue(0x05, bitValLo, 0xffff)) {
		printf("SetWireInLo failed.\n");
		//delete xem;
		return -1;
	}
    if (okCFrontPanel::NoError != this->xem -> SetWireInValue(0x06, bitValHi, 0xffff)) {
		printf("SetWireInHi failed.\n");
		//delete xem;
		return -1;
    }  

    bitValLo = bitValM1Dystonia & 0xffff;
    bitValHi = (bitValM1Dystonia >> 16) & 0xffff;    
    //send muscle velocity to Fpga
    if (okCFrontPanel::NoError != this->xem -> SetWireInValue(0x07, bitValLo, 0xffff)) {
		printf("SetWireInLo failed.\n");
		//delete xem;
		return -1;
	}
    if (okCFrontPanel::NoError != this->xem -> SetWireInValue(0x08, bitValHi, 0xffff)) {
		printf("SetWireInHi failed.\n");
		//delete xem;
		return -1;
	}

    this->xem -> UpdateWireIns();
    this->xem -> ActivateTriggerIn(0x50, trigEvent);
    
    return 0;
}

