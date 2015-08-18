#pragma once
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include "NIDAQmx.h"

class analogClient
{
    static void staticAnalogClientCallback(void*);
    void analogClientCallback(void);
    TaskHandle  enableHandle;
    HANDLE hIOMutex;
    bool live;
    double delayThread;
    uInt32 messageToServer;
    int sendAnalogMessageToServer(uInt32);
    bool newMessage;
public:
    analogClient(void);
    ~analogClient(void);
    int sendMessageToServer(uInt32);
};

