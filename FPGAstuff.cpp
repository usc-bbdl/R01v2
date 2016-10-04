
void InitProgram()
{

    gAlterDamping = false;
    srand((unsigned) time(&randSeedTime));

    // Load Fpga DLLs
    char dll_date[32], dll_time[32];

    printf("---- Opal Kelly ---- FPGA-DES Application v1.0 ----\n");
    if (FALSE == okFrontPanelDLL_LoadLib(NULL)) {
        printf("FrontPanel DLL could not be loaded.\n");
        return;
    }
    okFrontPanelDLL_GetVersion(dll_date, dll_time);
    printf("FrontPanel DLL loaded.  Built: %s  %s\n", dll_date, dll_time);

    // Two muscles, each with one Fpga handle

    gXemSpindleBic = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "113700021E"); 
    gXemSpindleTri = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "11160001CG"); 
    gXemMuscleBic = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "0000000542") ; 
    gXemMuscleTri = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "1137000222") ; 


    gSwapFiles = new FileContainer;

    //IPP_VEL_IIR
    dlysVel0IIR = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));
    dlysVel1IIR = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));
    tapsVel0IIR  = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));
    tapsVel1IIR  = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));

    ippsZero_32f(dlysVel0IIR, 2 * (lenFilterVel_IIR + 1) );
    ippsZero_32f(dlysVel1IIR, 2 * (lenFilterVel_IIR + 1) );

    tapsVel0IIR[0] =  0.24212;  // for Lowpass filter velocity
    tapsVel0IIR[1] =  0.25788;
    tapsVel0IIR[2] =  0.25788;
    tapsVel0IIR[3] =  0.24212;
    tapsVel0IIR[4] =  1.00000;
    tapsVel0IIR[5] =  0.00000;
    tapsVel0IIR[6] =  0.00000;
    tapsVel0IIR[7] =  0.00000;

    tapsVel1IIR[0] =  0.24212;  // for Lowpass filter velocity
    tapsVel1IIR[1] =  0.25788;
    tapsVel1IIR[2] =  0.25788;
    tapsVel1IIR[3] =  0.24212;
    tapsVel1IIR[4] =  1.00000;
    tapsVel1IIR[5] =  0.00000;
    tapsVel1IIR[6] =  0.00000;
    tapsVel1IIR[7] =  0.00000;


    ippsIIRInitAlloc_32f(&pIIRStateVel0, tapsVel0IIR, lenFilterVel_IIR, dlysVel0IIR);
    ippsIIRInitAlloc_32f(&pIIRStateVel1, tapsVel1IIR, lenFilterVel_IIR, dlysVel1IIR);


    //IPP_FR
    taps0 = ippsMalloc_32f(lenFilter);
    taps1 = ippsMalloc_32f(lenFilter);
    dly0  = ippsMalloc_32f(lenFilter);
    dly1  = ippsMalloc_32f(lenFilter);

    float   P   =   1.0;
    float   e   =   2.7183;
    float   T   =   0.001;
    float   tau   =   0.090; // rising time of muscle twitch in seconds
    float   a   =   exp(-T / tau);
    float   pefat = P * e * T * a / tau;


    taps0[0] =   0.0000000 * pefat;
    taps0[1] =   1.00 * pefat;
    taps0[2] =   0.0000000 * pefat;
    taps0[3] =   1.00 * pefat;
    taps0[4] =  -2 * a * pefat;
    taps0[5] =   a * a * pefat;

    taps1[0] =   0.0000000 * pefat;
    taps1[1] =   1.00 * pefat;
    taps1[2] =   0.0000000 * pefat;
    taps1[3] =   1.00 * pefat;
    taps1[4] =  -2 * a * pefat;
    taps1[5] =   a * a * pefat;


    ippsZero_32f(dly0,lenFilter);
    ippsZero_32f(dly1,lenFilter);

    ippsIIRInitAlloc_32f( &pIIRState0, taps0, lenFilter, dly0 );
    ippsIIRInitAlloc_32f( &pIIRState1, taps1, lenFilter, dly1 );

    SineGen(gWave);
 

    //WARNING: DON'T CHANGE THE SEQUENCE BELOW
    StartReadPos(&gEncoderHandle[0], &gEncoderHandle[1]);


    StartSignalLoop(&gAOTaskHandle, &gForceReadTaskHandle); 
    InitMotor(&gCurrMotorState);

}




void* SpindleLoopBic(void*)
{
    while (true)
    {
        if(GetAsyncKeyState(VK_SPACE))
        { 
            ShutdownMotor(&gCurrMotorState);
        }
        
        if ((MOTOR_STATE_RUN_PARADIGM != gCurrMotorState) && (MOTOR_STATE_CLOSED_LOOP != gCurrMotorState) && (MOTOR_STATE_OPEN_LOOP != gCurrMotorState)) continue;


        //*** Read from FPGA
        gXemSpindleBic->ReadFpga(0x22, "float32", &gSpindleIaBic);
        //gXemSpindleBic->ReadFpga(0x24, "float32", &gSpindleIIBic);        

        int32 bitValLce;
        ReInterpret((float32)(gMuscleLce[0]), &bitValLce);

        // *** Write back to FPGA
        gXemSpindleBic->SendPara(bitValLce, DATA_EVT_LCEVEL);

        //Sleep(1);
        if(_kbhit()) break;
    } 
    return 0;
}

void* SpindleLoopTri(void*)
{
    while (true)
    {
        if(GetAsyncKeyState(VK_SPACE))
        { 
            ShutdownMotor(&gCurrMotorState);
        }

        if ((MOTOR_STATE_RUN_PARADIGM != gCurrMotorState) && (MOTOR_STATE_CLOSED_LOOP != gCurrMotorState) && (MOTOR_STATE_OPEN_LOOP != gCurrMotorState)) continue;


        //*** Read from FPGA
        gXemSpindleTri->ReadFpga(0x22, "float32", &gSpindleIaTri);
        //gXemSpindleBic->ReadFpga(0x24, "float32", &gSpindleIIBic);        

        int32 bitValLce;
        ReInterpret((float32)(gMuscleLce[1]), &bitValLce);

        // *** Write back to FPGA
        gXemSpindleTri->SendPara(bitValLce, DATA_EVT_LCEVEL);

        //Sleep(1);
        if(_kbhit()) break;
    } 
    return 0;
}


void* ControlLoopBic(void*)
{

    int32       error=0;
    char        errBuff[2048]={'\0'};	

    long iLoop = 0;
    while (1)
    {
        if(GetAsyncKeyState(VK_SPACE))
        { 
            ShutdownMotor(&gCurrMotorState);
        }

        if ((MOTOR_STATE_RUN_PARADIGM != gCurrMotorState) && (MOTOR_STATE_CLOSED_LOOP != gCurrMotorState) && (MOTOR_STATE_OPEN_LOOP != gCurrMotorState)) continue;
        //printf("\n\t%f",dataEncoder[0]); 
        iLoop++;
        /*        if (10000 <= iLoop)
        {
        gM1Dystonia = 5000;
        gM1Voluntary = gWave[iLoop % 1024];
        }    */    

        //Read FPGA values for data Logging
        gXemMuscleBic->ReadFpga(0x20, "float32", &gEmgBic);
        //gXemMuscleBic->ReadFpga(0x30, "int32", &gSpikeCountBic);


        if(gAlterDamping && (gMusDamp>0.03f))
            //gMusDamp -= 0.03f;
            gMusDamp = 0.0f;


        //float32 tGainBic = 0.00005;// 0.051; // working = 0.141
        //float32 tGainTri = 0.00005;// 0.051; // working = 0.141
        float32 forceBiasBic = 10.0f;
        float   coef_damp = 0.004; // working = 0.04

        int32 bitValLce, bitValVel;
        int32   bitM1VoluntaryBic = 0, 
            bitM1DystoniaBic = 000;

        ReInterpret((float32)(gMusDamp * gMuscleVel[0]), &bitValVel);

        //*** Read from FPGA
        gXemMuscleBic->ReadFpga(0x32, "float32", &gForceBic);


        //*** Write back to Muscle_fpga
        ReInterpret((float32)(gMuscleLce[0]), &bitValLce);
        gXemMuscleBic->WriteFpgaLceVel(bitValLce, bitValVel, bitM1VoluntaryBic, bitM1DystoniaBic, DATA_EVT_LCEVEL);

        const float tBias = 0.0;
        float tCtrl = (gForceBic - tBias) * gGain;
        gCtrlFromFPGA[0] = (tCtrl >= 0.0) ? tCtrl : 0.0;
        //gCtrlFromFPGA[0] = 1.0;

        //Sleep(1);
        if(_kbhit()) break;
    } 
    return 0;
}

void* ControlLoopTri(void*)
{

    int32       error=0;
    char        errBuff[2048]={'\0'};

    long iLoop = 0;
    while (1)
    {
        if(GetAsyncKeyState(VK_SPACE))
        { 
            ShutdownMotor(&gCurrMotorState);
        }

        if ((MOTOR_STATE_RUN_PARADIGM != gCurrMotorState) && (MOTOR_STATE_CLOSED_LOOP != gCurrMotorState) && (MOTOR_STATE_OPEN_LOOP != gCurrMotorState)) continue;
        //printf("\n\t%f",dataEncoder[0]); 
        iLoop++;
        /*        if (10000 <= iLoop)
        {
        gM1Dystonia = 5000;
        gM1Voluntary = gWave[iLoop % 1024];
        }    */    

        //Read FPGA values for data Logging
        gXemMuscleTri->ReadFpga(0x20, "float32", &gEmgTri);
        //gXemMuscleTri->ReadFpga(0x30, "int32", &gSpikeCountTri);

        float32 forceBiasTri = 10.0f;
        float   coef_damp = 0.004; // working = 0.04

        int32 bitValLce, bitValVel;
        int32   bitM1VoluntaryTri = 0, 
            bitM1DystoniaTri = 000;

        ReInterpret((float32)(gMuscleLce[1]), &bitValLce);
        ReInterpret((float32)(gMusDamp * gMuscleVel[1]), &bitValVel);

        gXemMuscleTri->ReadFpga(0x32, "float32", &gForceTri);
        gXemMuscleTri->WriteFpgaLceVel(bitValLce, bitValVel, bitM1VoluntaryTri, bitM1DystoniaTri, DATA_EVT_LCEVEL);

        const float tBias = 0.0;//9000000.0;  
        float tCtrl = (gForceTri - tBias) * gGain;
        gCtrlFromFPGA[1] = (tCtrl >= 0.0) ? tCtrl : 0.0;
        //gCtrlFromFPGA[1] = 1.0;


        //Sleep(1);
        if(_kbhit()) break;
    } 
    return 0;
}


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



SomeFpga::SomeFpga(int NUM_NEURON = 128, int SAMPLING_RATE = 1024, std::string serX = "")
{
    this->NUM_NEURON = NUM_NEURON;
    this->SAMPLING_RATE = SAMPLING_RATE;
    
    this->xem = new okCFrontPanel;

    std::cout << "Connecting to OpaKelly of serial number: " << serX << std::endl;
    std::cout<<"Kian HERE"<<std::endl;
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