#include "analogClient.h"
#include "NIDAQmx.h"
#include "utilities.h"



analogClient::analogClient(void)
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    DAQmxErrChk (DAQmxCreateTask("",&enableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(enableHandle,"PXI1Slot8/port0","motorEnable",DAQmx_Val_ChanForAllLines));
    DAQmxErrChk (DAQmxStartTask(enableHandle));
    messageToServer = MESSAGE_KINEMATIC;
    analogClient::sendAnalogMessageToServer(messageToServer);
	//hIOMutex = CreateMutex(NULL, FALSE, NULL);
	//live = TRUE;
    //newMessage = FALSE;
    delayThread = 50;
	//_beginthread(analogClient::staticAnalogClientCallback, 0, this);

    Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(enableHandle);
		DAQmxClearTask(enableHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("Analog client initialization error\n");
        live = FALSE;
	}
}
void analogClient::staticAnalogClientCallback(void *a) 
{
    ((analogClient *)a)->analogClientCallback();
}


analogClient::~analogClient(void)
{
    messageToServer = MESSAGE_NO_CONNECTION;
    analogClient::sendAnalogMessageToServer(MESSAGE_NO_CONNECTION);
    Sleep(200);
    live = FALSE;
    DAQmxStopTask(enableHandle);
	DAQmxClearTask(enableHandle);
}

void analogClient::analogClientCallback(void)
{
    while (live){
        WaitForSingleObject(hIOMutex, INFINITE);
        Sleep(delayThread);
        if (newMessage = TRUE)
        {
            sendAnalogMessageToServer(messageToServer);
            newMessage = FALSE;
        }
        ReleaseMutex( hIOMutex);
    }
}

int analogClient::sendAnalogMessageToServer(uInt32 message)
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    DAQmxErrChk (DAQmxWriteDigitalU32(enableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&message,NULL,NULL));
    if (message == MESSAGE_NO_CONNECTION)
    {
        messageToServer = MESSAGE_NO_CONNECTION;
    }else{
        Sleep(1500);
        DAQmxErrChk (DAQmxWriteDigitalU32(enableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&MESSAGE_IDLE,NULL,NULL));
        messageToServer = MESSAGE_IDLE;
    }
    
Error:
	if( DAQmxFailed(error) ) {
        live = FALSE;
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(enableHandle);
		DAQmxClearTask(enableHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("Callback Error\n");
        printf("Analog client transmission error\n");
	}
    return 0;
}

int analogClient::sendMessageToServer(uInt32 message)
{
    newMessage = TRUE;
    messageToServer = message;
    sendAnalogMessageToServer (messageToServer);
    return 0;
}