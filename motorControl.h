#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <NIDAQmx.h>
#include <utilities.h>
#include <iostream>
class motorControl
{
    TaskHandle  motorTaskHandle, motorEnableHandle;
public:
    motorControl();
    ~motorControl(void);
    bool isEnable, isWindUp, isControlled;
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorController();// FROM SURAJ
};

#endif