// experimentUserInterface.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>

#include "utilities.h"
//#include "okFrontPanelDLL.h"
//#include "servoControl.h"
//#include "motorControl.h"
//#include "expParadigmMuscleLengthCalibration.h"
//#include "expParadigmServoPerturbation.h"
//#include "expParadigmManualPerturbation.h"
//#include "expParadigmVoluntaryMovement.h"
#include "AdeptArmAPI.h"

int main()
{
    PPoint defaultPoint, newPoint;
    AdeptArmAPI adeptRobot;
    // Get the default position using where command in V+ terminal
    // Get the configuration space not endpoint space , i.e. the numbers below are joint angles
    defaultPoint.x = 80.0;
    defaultPoint.y = -56.0;
    defaultPoint.z = 143.0;
    defaultPoint.a = -98.0;
    defaultPoint.b = -116.0;
    defaultPoint.c = 131.0;
    int numberOfPerturbations = 10;
    adeptRobot.connectToController();
    double perturbationAngle = 20;
    long double angle[6];

    angle[0] = defaultPoint.x;
    angle[1] = defaultPoint.y;
    angle[2] = defaultPoint.z;
    angle[3] = defaultPoint.a;
    angle[4] = defaultPoint.b;
    angle[5] = defaultPoint.c;

    double flexAngle = perturbationAngle; 
    double extendAngle = -perturbationAngle;
    bool isFlex = true;
    //adeptRobot.setVelocity (10, 10, MONITOR, true);
    for (int i = 0; i<numberOfPerturbations; i++)
    {
        printf("i = %d\n",i);
        adeptRobot.move(defaultPoint);
        angle[0] = defaultPoint.x;
        angle[1] = defaultPoint.y;
        angle[2] = defaultPoint.z;
        angle[3] = defaultPoint.a;
        angle[4] = defaultPoint.b;
        angle[5] = defaultPoint.c;
        if (isFlex==true)
        {
            angle[5] = angle[5] + flexAngle;
            isFlex = false;
        }
        else if (isFlex==false)
        {
            angle[5] = angle[5] + extendAngle;
            isFlex = true;
        }
        newPoint = PPoint(angle[0],angle[1],angle[2],angle[3],angle[4],angle[5]);
        adeptRobot.move(newPoint);
        Sleep(1000);
    }

    adeptRobot.move(defaultPoint);    
    getch();
    return 0;
}
