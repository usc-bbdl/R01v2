#include "motorControl.h"
#include <utilities.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
motorControl::motorControl(double offset0, double offset1, double offset2, double offset3, double offset4, double offset5, double offset6)
{
    //
    newPdgm_Flag = false;
    newPdgm_ref[0] = newPdgm_ref[1] = newPdgm_ref[2] = newPdgm_ref[3] = newPdgm_ref[4] = newPdgm_ref[5] = newPdgm_ref[6] = 0;
    //
    dataEnable = 0x7F;//0x8111111F;//0x1111111;
    encoderBias[0] = encoderBias[1] = encoderBias[2] = 0;
    encoderGain[0] = encoderGain[1] = encoderGain[2] = 1;
    perturbationAngle = 0;
    I = 3;
    cortexVoluntaryAmp = 10000;
    cortexVoluntaryFreq = 0.25;
    char        errBuff[2048]={'\0'};
    int32       error=0;
    angle = 0;
    velocity = 0;
    trialTrigger = 0;
    gammaStatic1 = 0;
    gammaDynamic1 = 0;
    gammaStatic2 = 0;
    gammaDynamic2 = 0;
    cortexDrive[0] = 0;
    cortexDrive[1] = 0;
    spindleIa[0] = 0;
    spindleII[0] = 0;
    spindleIa[1] = 0;
    spindleII[1] = 0;
    isEnable = FALSE;
    isWindUp = FALSE;
    isControlling = FALSE;
    live = FALSE;

    loadCellOffset0 = offset0;
    loadCellOffset1 = offset1;
    loadCellOffset2 = offset2;
    loadCellOffset3 = offset3;
    loadCellOffset4 = offset4;
    loadCellOffset5 = offset5;
    loadCellOffset6 = offset6;

    std::cout<<"\n\nLC offsets:\n\t0: "<<loadCellOffset0
                            <<"\n\t1: "<<loadCellOffset1
                            <<"\n\t2: "<<loadCellOffset2
                            <<"\n\t3: "<<loadCellOffset3
                            <<"\n\t4: "<<loadCellOffset4
                            <<"\n\t5: "<<loadCellOffset5
                            <<"\n\t6: "<<loadCellOffset6<<std::endl;
    loadCellData[0] = 0;
    loadCellData[1] = 0;
    loadCellData[2] = 0;
    loadCellData[3] = 0;
    loadCellData[4] = 0;
    loadCellData[5] = 0;
    loadCellData[6] = 0;
    loadCellData[7] = 0;

    motorRef[0] = 5;
    motorRef[1] = 5;
    motorRef[2] = 5;
    motorRef[3] = 5;
    motorRef[4] = 5;
    motorRef[5] = 5;
    motorRef[6] = 5;

    encoderData1[0] = 0;
    encoderData2[0] = 0;
    encoderData3[0] = 0;

    resetMuscleLength = TRUE;
    muscleLengthPreviousTick[0] = 1;
    muscleLengthPreviousTick[1] = 1;
    muscleLengthOffset [0] = 0;
    muscleLengthOffset [1] = 0;
    strcpy(header,"Time, Clock, Exp Prot, Len1, ForcMeas, ForcRef, Digit Force 1, Digit Force2, Digit Contact 1, Digit Contact 2");
    if (dataAcquisitionFlag[0]){
        //strcat (header, "");
    }
    if (dataAcquisitionFlag[1]){
        strcat (header, ", EMG1, EMG2");
    }
    if (dataAcquisitionFlag[2]){
        strcat (header, ", Ia1, Ia2");
    }
    if (dataAcquisitionFlag[3]){
        strcat (header, ", II1, II2");
    }
    if (dataAcquisitionFlag[4]){
        strcat (header, ", Spike Count1, Spike Count2");
    }
    if (dataAcquisitionFlag[5]){
        strcat (header, ", Raster 1-1,  Raster 2-1");
    }
    if (dataAcquisitionFlag[6]){
        strcat (header, ", Raster 1-2,  Raster 2-2");
    }
    if (dataAcquisitionFlag[7]){
        strcat (header, ", Raster 1-3,  Raster 2-3");
    }
    if (dataAcquisitionFlag[8]){
        strcat (header, ", Raster 1-4,  Raster 2-4");
    }
    if (dataAcquisitionFlag[9]){
        strcat (header, ", Raster 1-5,  Raster 2-5");
    }
    if (dataAcquisitionFlag[10]){
        strcat (header, ", Raster 1-6,  Raster 2-6");
    }
    char dataTemp[100]="";
    strcat(header,"\n");
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[0],dataAcquisitionFlag[1],dataAcquisitionFlag[2],dataAcquisitionFlag[3]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[4],dataAcquisitionFlag[5],dataAcquisitionFlag[6],dataAcquisitionFlag[7]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d\n",dataAcquisitionFlag[8],dataAcquisitionFlag[9],dataAcquisitionFlag[10],dataAcquisitionFlag[11]);
    strcat(header,dataTemp);


    //----------
    // LOAD CELL
    //----------
    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai0" ,"loadCell0",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai8" ,"loadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai1" ,"loadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai9" ,"loadCell3",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai2" ,"loadCell4",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai10","loadCell5",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai11","loadCell6",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));    
    /*
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai22","directRecordingFromDigiOut",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai24","digit force 1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai25","digit force 2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai26","digit contact 1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai27","digit contact 2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    */
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    DAQmxErrChk (DAQmxSetRealTimeConvLateErrorsToWarnings(loadCelltaskHandle,1));


    //-------------
    // MOTOR ANALOG
    //-------------
    DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao8" ,"motor0",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao9" ,"motor1",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));//old: ao11
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao10","motor2",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));//old: ao11
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor3",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));//old: ao11
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao12","motor4",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));//old: ao11
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao13","motor5",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));//old: ao11
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao14","motor6",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));//old: ao11
    /*
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao31","speaker",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao30","clock",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    */
    DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    

    //-------------
    // MOTOR ENABLE
    //-------------
    DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","motorEnable",DAQmx_Val_ChanForAllLines));


    //--------
    // ENCODER
    //--------
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[0]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[0],"PXI1Slot3/ctr0","Enoder 0",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[1]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[1],"PXI1Slot3/ctr1","Enoder 1",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));//old: ctr3
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[1],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));    
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[2]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[2],"PXI1Slot3/ctr2","Enoder 2",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));//old: ctr3
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[2],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));


Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(motorTaskHandle);
		DAQmxClearTask(loadCelltaskHandle);
		DAQmxClearTask(motorEnableHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor, load cell or encoder initialization error\n");
	}
}

motorControl::~motorControl()
{
    DAQmxClearTask(motorEnableHandle);
    DAQmxClearTask(motorTaskHandle);
    DAQmxClearTask(loadCelltaskHandle);
    live = FALSE;
}

int motorControl::motorEnable()
{
    
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 zeroVoltages[7]={0.0,0.0,0.0,0.0,0.0,0.0,0.0}, zeroVoltage={0.0};
    DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,NULL,NULL));
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    Sleep(500);
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isEnable = TRUE;
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);

		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor enable Error\n");
	}
    return 0;
}

int motorControl::motorWindUp()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 windingUpCmnd[7]={0.5,0.5,0.5,0.5,0.5,0.5,0.5};
    if (isEnable){
        DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,windingUpCmnd,NULL,NULL));
        Sleep(500);
        isWindUp = TRUE;

        DAQmxStopTask(motorTaskHandle);
        
        printf("Windup Pass.\n");
    }else  printf("Motors must be first enabled prior to winding up.\n");

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
        DAQmxClearTask(motorTaskHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("winding up Error\n");
	}
     return 0;
}

void motorControl::motorControlLoop(void* a)
{
	((motorControl*)a)->controlLoop();
}

void motorControl::controlLoop(void)
{
    int32       error=0;
    float cotexDrive = 0.0;
    bool keepReading=TRUE;
    bool32 isLate = {0};
    double tick=0.0,tock=0.0;
    float64 motorCommand[7] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
              errorForce[7] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                integral[7] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                     EMG    = {0.0};
    char        errBuff[2048]={'\0'};
    FILE *dataFile;
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char fileName[200];
    char dataSample[600]="";
    char dataTemp[100]="";
    sprintf(dataTemp,"\n");
    sprintf_s(
            fileName,
            "C:\\data\\realTimeData%4d_%02d_%02d_%02d_%02d_%02d.txt",
            timePtr->tm_year+1900, 
            timePtr->tm_mon+1, 
            timePtr->tm_mday, 
            timePtr->tm_hour, 
            timePtr->tm_min, 
            timePtr->tm_sec
            );
    dataFile = fopen(fileName,"w");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Motor Command1, Motor Command2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, motorRef1, motorRef2, spindleIa1, spindleIa2, spindleII1, spindleII2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Length 1, Length2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    fprintf(dataFile,header);
    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[0]));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[1]));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[2]));
    DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    timeData.resetTimer();
    tick = timeData.getCurrentTime();
    float64 goffsetLoadCell[2]={0};
    int expProtocol = 0;
    int expProtocoAdvance = 0;
    uInt32 clockBit = 0x00000080;
    uInt32 clockBitXOR;
    clockBitXOR = ((clockBit>>7) | (dataEnable>>31));
    bool flg = true;
    
    DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate));
    DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,8,NULL,NULL));
    printf("\n\nLOOP: Raw LC Offsets:\n\t0: %2.4f, 1: %2.4f, 2: %2.4f, 3: %2.4f,\n\t4: %2.4f, 5: %2.4f, 6: %2.4f .\n\n",
            loadCellData[0],loadCellData[1],loadCellData[2],loadCellData[3],loadCellData[4],loadCellData[5],loadCellData[6]);
    loadCellOffset0 = loadCellData[0] * loadCellScale0;
    loadCellOffset1 = loadCellData[1] * loadCellScale1;
    loadCellOffset2 = loadCellData[2] * loadCellScale2;
    loadCellOffset3 = loadCellData[3] * loadCellScale3;
    loadCellOffset4 = loadCellData[4] * loadCellScale4;
    loadCellOffset5 = loadCellData[5] * loadCellScale5;
    loadCellOffset6 = loadCellData[6] * loadCellScale6;
    printf("\n\nScaled LC Offsets:\n\t0: %2.4f, 1: %2.4f, 2: %2.4f, 3: %2.4f,\n\t4: %2.4f, 5: %2.4f, 6: %2.4f .\n\n",
            loadCellOffset0,loadCellOffset1,loadCellOffset2,loadCellOffset3,loadCellOffset4,loadCellOffset5,loadCellOffset6);

    while(live)
    {
        if(flg)
        {
        clockBitXOR = 0xff;
        flg = false;
        }
        else
        {
            clockBitXOR = 0x01111111;
            flg = true;
        }
        if (dataEnable == 0x07)
            dataEnable = 0xFF;
        else
            dataEnable = 0x7F;
        //else if (dataEnable == 71)
        //    dataEnable = 7;


        //printf("\n\n\n\n%lf\n\n\n",(double)loadCellData[0]);


        //DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&clockBitXOR,NULL,NULL));
        DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,NULL,NULL));
        WaitForSingleObject(hIOMutex, INFINITE);
        //desire Forced, muscle Length, muscle Velocity PIPES should be read here
        
        DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate));
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,8,NULL,NULL));
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[0],1,10.0,encoderData1,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[1],1,10.0,encoderData2,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[2],1,10.0,encoderData3,1,NULL,0));
        /*
        if (dataAcquisitionFlag[1]){
            EMG = muscleEMG[0];
            if (EMG > 6)
                EMG = 6;
            if (EMG < -6)
                EMG = -6;
        }
        else
            EMG = 0;
        */


        //printf("\n\n\n\n%lf\n\n\n",(double)loadCellData[0]);


        tock = timeData.getCurrentTime();
        if (resetMuscleLength)
        {
            muscleLengthOffset[0] = 2 * PI * shaftRadius * encoderData1[0] / 365;
            muscleLengthOffset[1] = 2 * PI * shaftRadius * encoderData2[0] / 365;
            muscleLengthOffset[2] = 2 * PI * shaftRadius * encoderData3[0] / 365;
            resetMuscleLength = FALSE;
        }
        muscleLength[0] = ((2 * PI * shaftRadius * encoderData1[0] / 365) - muscleLengthOffset[0]);
        muscleLength[0] = encoderBias[0] + muscleLength[0] *encoderGain[0];
        muscleLength[1] = ((2 * PI * shaftRadius * encoderData2[0] / 365) - muscleLengthOffset[1]);
        muscleLength[1] = encoderBias[1] + muscleLength[1] *encoderGain[1];

        muscleLength[2] = ((2 * PI * shaftRadius * encoderData3[0] / 365) - muscleLengthOffset[2]);
        muscleLength[2] = encoderBias[2] + muscleLength[2] *encoderGain[2];

        muscleVel[0] = (muscleLength[0] -  muscleLengthPreviousTick[0]) / (tock - tick);
        muscleVel[1] = (muscleLength[1] -  muscleLengthPreviousTick[1]) / (tock - tick);

        muscleLengthPreviousTick[0] = muscleLength[0];
        muscleLengthPreviousTick[1] = muscleLength[1];
        
        loadCellData[0] = (loadCellData[0] * loadCellScale0) - loadCellOffset0;
        loadCellData[1] = (loadCellData[1] * loadCellScale1) - loadCellOffset1;
        loadCellData[2] = (loadCellData[2] * loadCellScale2) - loadCellOffset2;
        loadCellData[3] = (loadCellData[3] * loadCellScale3) - loadCellOffset3;
        loadCellData[4] = (loadCellData[4] * loadCellScale4) - loadCellOffset4;
        loadCellData[5] = (loadCellData[5] * loadCellScale5) - loadCellOffset5;
        loadCellData[6] = (loadCellData[6] * loadCellScale6) - loadCellOffset6;
        
        if(newPdgm_Flag)
        {
            //errorForce[0] = newPdgm_ref[1] - loadCellData[0];
            //errorForce[1] = newPdgm_ref[0] - loadCellData[1];
            //errorForce[2] = newPdgm_ref[0] - loadCellData[2];
            motorRef[0] = newPdgm_ref[0];
            motorRef[1] = newPdgm_ref[1];
            motorRef[2] = newPdgm_ref[2];
            motorRef[3] = newPdgm_ref[3];
            motorRef[4] = newPdgm_ref[4];
            motorRef[5] = newPdgm_ref[5];
            motorRef[6] = newPdgm_ref[6];
        }
        errorForce[0] = motorRef[0] - loadCellData[0];
        errorForce[1] = motorRef[1] - loadCellData[1];
        errorForce[2] = motorRef[2] - loadCellData[2];
        errorForce[3] = motorRef[3] - loadCellData[3];
        errorForce[4] = motorRef[4] - loadCellData[4];
        errorForce[5] = motorRef[5] - loadCellData[5];
        errorForce[6] = motorRef[6] - loadCellData[6];

        integral[0] = integral[0] + errorForce[0] * (tock - tick);
        integral[1] = integral[1] + errorForce[1] * (tock - tick);
        integral[2] = integral[2] + errorForce[2] * (tock - tick);
        integral[3] = integral[3] + errorForce[3] * (tock - tick);
        integral[4] = integral[4] + errorForce[4] * (tock - tick);
        integral[5] = integral[5] + errorForce[5] * (tock - tick);
        integral[6] = integral[6] + errorForce[6] * (tock - tick);

        motorCommand[0] = integral[0] * I;
        motorCommand[1] = integral[1] * 0;
        motorCommand[2] = integral[2] * 0;
        motorCommand[3] = integral[3] * 0;
        motorCommand[4] = integral[4] * 0;
        motorCommand[5] = integral[5] * 0;
        motorCommand[6] = integral[6] * 0;

        //motorCommand[3] = EMG;

        if (motorCommand[0] > motorMaxVoltage)
            motorCommand[0] = motorMaxVoltage;
        if (motorCommand[0] < motorMinVoltage)
            motorCommand[0] = motorMinVoltage;

        if (motorCommand[1] > motorMaxVoltage)
            motorCommand[1] = motorMaxVoltage;
        if (motorCommand[1] < motorMinVoltage)
            motorCommand[1] = motorMinVoltage;

        if (motorCommand[2] > motorMaxVoltage)
            motorCommand[2] = motorMaxVoltage;
        if (motorCommand[2] < motorMinVoltage)
            motorCommand[2] = motorMinVoltage;

        if (motorCommand[3] > motorMaxVoltage)
            motorCommand[3] = motorMaxVoltage;
        if (motorCommand[3] < motorMinVoltage)
            motorCommand[3] = motorMinVoltage;

        if (motorCommand[4] > motorMaxVoltage)
            motorCommand[4] = motorMaxVoltage;
        if (motorCommand[4] < motorMinVoltage)
            motorCommand[4] = motorMinVoltage;

        if (motorCommand[5] > motorMaxVoltage)
            motorCommand[5] = motorMaxVoltage;
        if (motorCommand[5] < motorMinVoltage)
            motorCommand[5] = motorMinVoltage;

        if (motorCommand[6] > motorMaxVoltage)
            motorCommand[6] = motorMaxVoltage;
        if (motorCommand[6] < motorMinVoltage)
            motorCommand[6] = motorMinVoltage;
        
        printf("L0: %+3.2f; L1: %+3.2f; L2: %+3.2f; L3: %+3.2f; L4: %+3.2f; L5: %+3.2f; L6: %+3.2f\r",loadCellData[0],loadCellData[1],loadCellData[2],loadCellData[3],loadCellData[4],loadCellData[5],loadCellData[6]);
        ReleaseMutex( hIOMutex);
        sprintf(dataSample,"%.3f,%d,%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",tock,flg, expProtocol,muscleLength[0], loadCellData[0], motorRef[0],loadCellData[4],loadCellData[5],loadCellData[6],loadCellData[7]);
        strcat (dataSample, dataTemp);
        if (dataAcquisitionFlag[0]){
            //sprintf(dataTemp,",%.6f,%.6f",motorRef[0],motorCommand[0]);
            //strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[1]){
            sprintf(dataTemp,",%.6f,%.6f",muscleEMG[0], muscleEMG[1]);
            strcat (dataSample, dataTemp);
        }
         if (dataAcquisitionFlag[2]){
            sprintf(dataTemp,",%.6f,%.6f",spindleIa[0], spindleIa[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[3]){
            sprintf(dataTemp,",%.6f,%.6f",spindleII[0], spindleII[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[4]){
            sprintf(dataTemp,",%d,%d",muscleSpikeCount[0], muscleSpikeCount[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[5]){
            sprintf(dataTemp,",%u,%u",raster_MN_1[0], raster_MN_1[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[6]){
            sprintf(dataTemp,",%u,%u",raster_MN_2[0], raster_MN_2[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[7]){
            sprintf(dataTemp,",%u,%u",raster_MN_3[0], raster_MN_3[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[8]){
            sprintf(dataTemp,",%u,%u",raster_MN_4[0], raster_MN_4[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[9]){
            sprintf(dataTemp,",%u,%u",raster_MN_5[0], raster_MN_5[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[10]){
            sprintf(dataTemp,",%u,%u",raster_MN_6[0], raster_MN_6[1]);
            strcat (dataSample, dataTemp);
        }
        if (dataAcquisitionFlag[11]){
            cortexDrive[0] = max((cortexVoluntaryAmp -0) * sin (2 * 3.1416 * cortexVoluntaryFreq * tick), 0);
            cortexDrive[1] = max((cortexVoluntaryAmp -0) * sin (2 * 3.1416 * cortexVoluntaryFreq * tick + 3.1416), 0);
        }
        //sprintf(dataTemp,",%d,%d,%d,%d,%.3f,%.3f,%d\n",gammaStatic1, gammaDynamic1, gammaStatic2, gammaDynamic2, cortexDrive[0], cortexDrive[1],newTrial);
        if (trialTrigger == 1){
            expProtocoAdvance = 1;
            trialTrigger = 0;
        }
        if (trialTrigger == 2){
            expProtocoAdvance = 1;
            trialTrigger = 0;
        }
        if (trialTrigger == 3){
            expProtocoAdvance = 11;
            trialTrigger = 0;
        }
        expProtocol = 0;
        switch(expProtocoAdvance){
            case 1:
                expProtocol = -1000;
                expProtocoAdvance = 2;
                break;
            case 2:
                expProtocol = perturbationAngle;
                expProtocoAdvance = 3;
                break;
            case 3:
                expProtocol = motorRef[0];
                expProtocoAdvance = 4;
                break;
            case 4:
                expProtocol =  motorRef[1];
                expProtocoAdvance = 0;
                break;
        }
        fprintf(dataFile,dataSample);
        tick = timeData.getCurrentTime();

    }

    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isControlling = FALSE;
    fclose(dataFile);
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(loadCelltaskHandle);
		DAQmxClearTask(loadCelltaskHandle);

        DAQmxStopTask(encodertaskHandle[0]);
        DAQmxStopTask(encodertaskHandle[1]);
        DAQmxStopTask(encodertaskHandle[2]);
		DAQmxClearTask(encodertaskHandle[0]);
        DAQmxClearTask(encodertaskHandle[1]);
        DAQmxClearTask(encodertaskHandle[2]);

        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);

        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);

		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor control Error\n");
	}
}

int motorControl::motorControllerStart()
{
    if ((isEnable) && (isWindUp))
    {
        isControlling = TRUE;
        live = TRUE;
        hIOMutex = CreateMutex(NULL, FALSE, NULL);
		isControlling = TRUE;
		_beginthread(motorControl::motorControlLoop,0,this);
    }else
    {
        isControlling = FALSE;
        printf("Motors must be first enabled or wind up before closed-loop control.\n");
    }
    return 0;
}
int motorControl::motorControllerEnd()
{
    live = FALSE;
    Sleep(500);
    motorControl::motorDisable();
    isControlling = FALSE;
    DAQmxStopTask(motorTaskHandle);
	DAQmxClearTask(motorTaskHandle);

    DAQmxStopTask(loadCelltaskHandle);
	DAQmxClearTask(loadCelltaskHandle);

    DAQmxStopTask(encodertaskHandle[0]);
    DAQmxStopTask(encodertaskHandle[1]);
    DAQmxStopTask(encodertaskHandle[2]);
	DAQmxClearTask(encodertaskHandle[0]);
    DAQmxClearTask(encodertaskHandle[1]);
    DAQmxClearTask(encodertaskHandle[2]);
    return 0;
}

int motorControl::motorDisable()
{
	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;
    float64 zeroVoltages[7]={0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    while(isControlling){
    }
    DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,NULL,NULL));
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    Sleep(500);
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isControlling = FALSE;
    isWindUp = FALSE;
    live = FALSE;
    isEnable = FALSE;

Error:
	if( DAQmxFailed(error) ){
		printf("DisableMotor Error: %s\n",errBuff);
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        printf("DAQmx Error: %s\n",errBuff);
		DAQmxStopTask(motorEnableHandle);
        DAQmxStopTask(motorTaskHandle);
        DAQmxClearTask(motorEnableHandle);
        DAQmxClearTask(motorTaskHandle);
    }
	return 0;
}

int motorControl::setPerturbationAngle(double perturbationAngle)
{
    this->perturbationAngle = perturbationAngle;
    return 1;
}


double motorControl::getTime()
{
    return timeData.getCurrentTime();
}
TimeData::TimeData(void)
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&initialTick);
}


TimeData::~TimeData(void)
{
}

// Reset the timer
int TimeData::resetTimer(void)
{
    QueryPerformanceCounter(&initialTick);
    return 0;
}


// Get current time in seconds
double TimeData::getCurrentTime(void)
{
    QueryPerformanceCounter(&currentTick);
    actualTime = (double)(currentTick.QuadPart - initialTick.QuadPart);
    actualTime /= (double)frequency.QuadPart;
    return actualTime;
}
