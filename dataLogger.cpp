#include "dataLogger.h"

dataLogger::dataLogger()
{
    int32       error=0;
	char        errBuff[2048]={'\0'};

    encodertaskHandle[0] = 0;
    encodertaskHandle[1] = 0;
    loadCelltaskHandle = 0;
    samplingFrequency = sampleFreq;
    isRecording = FALSE;
    fileName = "dataLogExperiment.txt";
    trialLength = 10;
    loadCellOffset1 = 0;
    loadCellOffset2 = 0;
    totalSampleRead = 0;
    
    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[0]));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[1]));

    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai0","Load Cell 1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai1","Load Cell 2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",samplingFrequency,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplingFrequency*2));
    
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[0],"PXI1Slot3/ctr4","Enoder 1",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,500,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot5/ai/SampleClock",samplingFrequency,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplingFrequency));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[1],"PXI1Slot3/ctr5","Encoder 2",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,500,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[1],"/PXI1Slot5/ai/SampleClock",samplingFrequency,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplingFrequency));

Error:
	if( DAQmxFailed(error) ){
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	    printf("DAQmx Error: %s\n",errBuff);
    }
}

dataLogger::~dataLogger()
{
}
int dataLogger::reset()
{
    int32       error=0;
	char        errBuff[2048]={'\0'};
    totalSampleRead = 0;

    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[0]));
    DAQmxErrChk (DAQmxCreateTask("",&encodertaskHandle[1]));

    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai0","Load Cell 1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai1","Load Cell 2",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",samplingFrequency,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplingFrequency*2));
    
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[0],"PXI1Slot3/ctr4","Enoder 1",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,500,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[0],"/PXI1Slot5/ai/SampleClock",samplingFrequency,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplingFrequency));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encodertaskHandle[1],"PXI1Slot3/ctr5","Encoder 2",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,500,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encodertaskHandle[1],"/PXI1Slot5/ai/SampleClock",samplingFrequency,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplingFrequency));

Error:
	if( DAQmxFailed(error) ){
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	    printf("DAQmx Error: %s\n",errBuff);
    }
    return 0;
    
}

int dataLogger::startRecording()
{
    int32       error=0;
	char        errBuff[2048]={'\0'};
    isRecording = TRUE;
    dataFile = fopen(fileName,"w");
    if (dataFile == NULL) {
        printf("Could not open data file to write");
    }
    int32       readLoadCell,readEncoder1,readEncoder2=0;
    int32       numRead=0;
	//float64     loadCellData[sampleFreq*2]={0.0};
    float64* loadCellData  = NULL;  
    float64* encoderData1  = NULL;  
    float64* encoderData2  = NULL;  
    loadCellData  = new float64[sampleFreq*2];
    encoderData1  = new float64[sampleFreq];
    encoderData2  = new float64[sampleFreq];
    //float64     encoderData1[sampleFreq]={0.0},encoderData2[sampleFreq]={0.0};

    fprintf(dataFile,"Load Cell1,Load Cell2,Muscle Length 1, Muscle Length2\n");

    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[0]));
    DAQmxErrChk (DAQmxStartTask(encodertaskHandle[1]));
    printf("Recording data for %d s\n",trialLength);
    printf("Pressing the escape key will make premature termination of data acquisition\n");
    while (totalSampleRead<(trialLength*samplingFrequency))
    {
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,samplingFrequency,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,samplingFrequency*10,&readLoadCell,NULL));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[0],samplingFrequency,10.0,encoderData1,samplingFrequency*5,&readEncoder1,0));
        DAQmxErrChk (DAQmxReadCounterF64(encodertaskHandle[1],samplingFrequency,10.0,encoderData2,samplingFrequency*5,&readEncoder2,0));
        if (!((readLoadCell==readEncoder1) && (readEncoder1==readEncoder2)))
            printf("DAQ Error: Samples are not synchronized %s\n");
        else if(readLoadCell>0){
            totalSampleRead+=readLoadCell;
            for(int i = 0; i<readLoadCell*2; i+=2)
            {
                loadCellData[i] = (loadCellData[i] *loadCellScale1) - loadCellOffset1;
                loadCellData[i+1] = (loadCellData[i+1] *loadCellScale2) - loadCellOffset2;
                encoderData1[i/2] = 2 * PI * shaftRadius * encoderData1[i/2] / 365 + 1;
                encoderData2[i/2] = 2 * PI * shaftRadius * encoderData2[i/2] / 365 + 1;
                fprintf(dataFile,"%.6f,%.6f,%.6f,%.6f\n",loadCellData[i],loadCellData[i+1],encoderData1[i/2],encoderData2[i/2]);
            }
            printf("Ld Cell1: %+6.2f; Ld Cell2: %+6.2fp; M Len 1: %+6.4f;M Len 2: %+6.4f\r",loadCellData[0],loadCellData[1],encoderData1[0],encoderData2[0]);
            fflush(stdout);
        }
        if(GetAsyncKeyState(VK_ESCAPE))
            break;
    }
    printf("\n Trial finished\n");
    delete [] loadCellData;  
    delete [] encoderData1;
    delete [] encoderData2;
    stopRecording();
Error:
	if( DAQmxFailed(error) ){
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	    printf("DAQmx Error: %s\n",errBuff);
    }
	return 0;
}

int dataLogger::stopRecording()
{
    fclose(dataFile);
    isRecording = FALSE;
    if( loadCelltaskHandle ) {
		DAQmxStopTask(loadCelltaskHandle);
		DAQmxClearTask(loadCelltaskHandle);
		loadCelltaskHandle = 0;
	}
    if( encodertaskHandle[0] ) {
		DAQmxStopTask(encodertaskHandle[0]);
		DAQmxClearTask(encodertaskHandle[0]);
		encodertaskHandle[0] = 0;
	}
    if( encodertaskHandle[1] ) {
		DAQmxStopTask(encodertaskHandle[1]);
		DAQmxClearTask(encodertaskHandle[1]);
		encodertaskHandle[1] = 0;
	}
    return 0;
}
