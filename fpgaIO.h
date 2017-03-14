#ifndef FPGAIO_HPP
#ifdef __cplusplus
#include <iostream>
#include <stdbool.h>
#include <stdint.h>
//#include "./include/utilities.h"
#ifdef _WIN64
  #include "../lib/FrontPanel/latest/nt64/okFrontPanelDLL.h"
#elif __linux__
  #include "../lib/FrontPanel/latest/ubuntu64/API/okFrontPanelDLL.h"
#endif

//Defintions for PLL clock sources
#define CLKREF  0
#define CLKSRC0 1
#define CLKSRC1 2
#define CLKSRC2 3
//advanced PLL/clock generator configurations
#define PLLRecruits 2
#define PLLOutCount 2
//options for manual or automatic configuration of FPGAs
#define MANUALCONF  0
#define AUTOCONF  1

int fpgaIOcheck(); //returns 0 if FrontPanel dynamic library is loaded; else returns 1

//errHandler handles device-driver specific errors
int errHandler(okCFrontPanel::ErrorCode, std::string); //error handling with user message
int errHandler(okCFrontPanel::ErrorCode); //error handling without user message

//lists connected OpalKelly devices and their serial numbers
int enumerateFPGA();

//struct for FPGA FMC bus settings
typedef struct {
  UINT32 FMCVoltageLevel;
  UINT32 FMCVoltageEnable;
  UINT32 FMCVoltageMode;
}settingsList;

//class for the fpgaIO class - one object is initialized per FPGA device being recruited
class fpgaIO{
  private:
    //
    //private member variables and flags
    //
    okCFrontPanel dev; //device descriptor
    std::string serialID; //device serial number
    std::string bitFile; //device bit file location
    okTDeviceInfo deviceInfo; //device information
    
    //device FMC settings
    okCDeviceSettings settings;
    settingsList listSettings;
    
    //device sensor states
    okTDeviceSensor  sensor ;
    okCDeviceSensors sensors;
    
    //Device Abstraction Layer (DAL) flags
                bool setPLL, setFMC, isFP3, designBurned, deviceReady;
    unsigned int selectUSB, usbSpeed, selectPLL;
    
    //
    //Private member functions
    //
    
    //bit-level re-interpretation of data for USB 2.0 bus
    uint32_t bitFormat(uint32_t data);

    int fpgaInit(std::string); //initializing the device
                int fpgaBurn(std::string); //burning the bit file
    int getDeviceInfo(); //Obtain device information
    
    //PLL clock generator settings functions
                int fpgaGetPLL150();
    int fpgaSetPLL150();
    int fpgaGetPLL393();
                int fpgaSetPLL393();

    int (*fpgaGetPLL)();
                int (*fpgaSetPLL)();

    
  public:
    //
    //public members
    //

    //PLL settings
    okCPLL22150 pll150; 
    okCPLL22393 pll393;
    fpgaIO(); //default constructor declaration
    fpgaIO(std::string serialNum, std::string bitfile, /*uint16_t designHASH,*/ unsigned int confType); // class constructor type 1
    ~fpgaIO(); //class destructor
    bool isDeviceReady();
    bool isDesignBurned();
    bool isPreConfig();

    //Clock generator routines
    double getReference();
    int setReference(double refFreq);
    double getPLLFreq(unsigned int PLLNum);
    int getPLLP(unsigned int PLLNum);
    int getPLLQ(unsigned int PLLNum);
    bool setPLLParameters(unsigned int PLLNum, int p, int q, bool enable);
    bool isPLLEnabled(unsigned int PLLNum);
    int getDivider(unsigned int outNum);
    bool setDivider(unsigned int outNum, int div);
    int getOutputSource(unsigned int outNum);
    bool setOutputSource(unsigned int outNum);
    bool isOutputEnabled(unsigned int outNum);
    bool setOutputEnable(unsigned int outNum, bool enable);
    int readClockGenSettings();
    int writeClockGenSettings();
    int readClockGenfromEEPROM();
    int loadClockGenfromEEPROM();
    
    //device FMC settings and sensor routines (USB 3.0 only)
    int readSettings(settingsList *list);
    int loadSettings(settingsList *list);   
    int readSensorStates(); // get device sensor state

    // I/O Primitives - returns ErrorCode - check the code to see if FPGA design is active
      //uses wires
    int readData (int *epAddr, int length, uint32_t *outVal);
    int writeData(int *epAddr, int length, uint32_t *inVal, uint32_t *mask);
      //uses triggers
    int getTrigger(int *epAddr, int length, bool *outVal, uint32_t *mask);
    int setTrigger(int epAddr, uint32_t bitVal);
      //uses pipes
    int readStream (int epAddr, long length, unsigned char *data);
    int writeStream(int epAddr, long length, unsigned char *data);
      //uses block-throttled pipes
    int readBlock (int epAddr, int blockSize, long length, unsigned char *data);
    int writeBlock(int epAddr, int blockSize, long length, unsigned char *data);
};

#endif // end of __cplusplus block
#endif // end of header declarations

/***********************\
|FRONTPANEL ERROR CODES:|
\********************** /
typedef enum {
  ok_NoError                    = 0,
  ok_Failed                     = -1,
  ok_Timeout                    = -2,
  ok_DoneNotHigh                = -3,
  ok_TransferError              = -4,
  ok_CommunicationError         = -5,
  ok_InvalidBitstream           = -6,
  ok_FileError                  = -7,
  ok_DeviceNotOpen              = -8,
  ok_InvalidEndpoint            = -9,
  ok_InvalidBlockSize           = -10,
  ok_I2CRestrictedAddress       = -11,
  ok_I2CBitError                = -12,
  ok_I2CNack                    = -13,
  ok_I2CUnknownStatus           = -14,
  ok_UnsupportedFeature         = -15,
  ok_FIFOUnderflow              = -16,
  ok_FIFOOverflow               = -17,
  ok_DataAlignmentError         = -18,
  ok_InvalidResetProfile        = -19,
  ok_InvalidParameter           = -20
} ok_ErrorCode;
*/
