#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <utilities.h>
#include <okFrontPanelDLL.h>
#include <string.h>
#include <assert.h>

bool    gAlterDamping = false;


class SomeFpga 
{
    public:
        SomeFpga(int , int , std::string);
        ~SomeFpga();
        int SendPara(int bitVal, int trigEvent);
        int SendButton(bool bitVal, int buttonEvt);
        int ReadFpga(BYTE getAddr, char *, float *);
        int ReadFpga(BYTE getAddr, char *,int *);
        //int ReadFpga(int getAddr);
        okCFrontPanel *xem;
        int WriteFpgaLceVel(int32, int32, int32, int32, int32);    
    private:
        int NUM_NEURON;
        int SAMPLING_RATE;
        char serX[50];
        


}; // Semi-colon is REQUIRED!


int main()
{
    //SomeFpga                *gXemSpindleBic; 
    //gXemSpindleBic = new SomeFpga(128, 1024, "113700021E"); 
    gAlterDamping = false;
    char dll_date[32], dll_time[32];
    printf("---- Opal Kelly ---- FPGA-DES Application v1.0 ----\n");
    //okFrontPanelDLL_LoadLib(NULL);
    /*if (FALSE == okFrontPanelDLL_LoadLib(NULL)) {
        printf("FrontPanel DLL could not be loaded.\n");
        return 0;
    }*/
    //okFrontPanelDLL_GetVersion(dll_date, dll_time);
    //printf("FrontPanel DLL loaded.  Built: %s  %s\n", dll_date, dll_time);

    printf("Press Esc to terminate\n");
    printf("Initialization; Next stage is Motors Winding up\n");
    int gExperimentState = MOTOR_STATE_INIT;
    bool stayInTheLoop = TRUE;
    while(stayInTheLoop)
    {
        char key = 0;
        if (kbhit()!=0){
            key = getch();
            switch ( key ) 
            {
                case 27:        // Terminate Anytime when Escape Is Pressed...
                    stayInTheLoop = FALSE;
                    //gExperimentState = MOTOR_STATE_RUN_PARADIGM;
                    //proceedState(&gExperimentState);
                    break;
                case ' ':       // Move forward in the state machine
                    //proceedState(&gExperimentState);
                    break;
           }
        }
    }
   return 0;
}



SomeFpga::SomeFpga(int NUM_NEURON = 128, int SAMPLING_RATE = 1024, std::string serX = "")
{
    this->NUM_NEURON = NUM_NEURON;
    this->SAMPLING_RATE = SAMPLING_RATE;
    
    this->xem = new okCFrontPanel;

    std::cout << "Connecting to OpaKelly of serial number: " << serX << std::endl;

    this->xem->OpenBySerial(serX);
    assert(this->xem->IsOpen());
//this->xem->LoadDefaultPLLConfiguration();

}

SomeFpga::~SomeFpga()
{
    delete this->xem;
}

int ReInterpret(float32 in, int32 *out)
{
    memcpy(out, &in, sizeof(int32));
    return 0;
}

int ReInterpret(int32 in, int32 *out)
{
    memcpy(out, &in, sizeof(int32));
    return 0;
}

int ReInterpret(int in, float *out)
{
    memcpy(out, &in, sizeof(float));
    return 0;
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