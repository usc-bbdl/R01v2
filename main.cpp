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
    EPoint defaultEPoint;
    AdeptArmAPI adeptRobot;
    // Get the default position using where command in V+ terminal
    // Get the configuration space not endpoint space , i.e. the numbers below are joint angles
    defaultPoint.J1 = 150;
    defaultPoint.J2 = -46.512;
    defaultPoint.J3 = 128.336;
    defaultPoint.J4 = -90;
    defaultPoint.J5 = -117.136;
    defaultPoint.J6 = 200.349;
    int numberOfPerturbations = 10;
    adeptRobot.connectToController();
    double perturbationAngle = 20;
    long double angle[6];
    adeptRobot.move(defaultPoint);    //added
   /* angle[0] = defaultPoint.J1;
    angle[1] = defaultPoint.J2;
    angle[2] = defaultPoint.J3;
    angle[3] = defaultPoint.J4;
    angle[4] = defaultPoint.J5;
    angle[5] = defaultPoint.J6;

    double flexAngle = perturbationAngle; 
    double extendAngle = -perturbationAngle;
    bool isFlex = true;
    //adeptRobot.setVelocity (10, 10, MONITOR, true);
    for (int i = 0; i<numberOfPerturbations; i++)
    {
        printf("i = %d\n",i);
        adeptRobot.move(defaultPoint);
        angle[0] = defaultPoint.J1;
        angle[1] = defaultPoint.J2;
        angle[2] = defaultPoint.J3;
        angle[3] = defaultPoint.J4;
        angle[4] = defaultPoint.J5;
        angle[5] = defaultPoint.J6;
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
 */
    defaultEPoint.X = -462;
    defaultEPoint.Y = 283.9;
    defaultEPoint.Z = 279;
    defaultEPoint.y = -103;
    defaultEPoint.p = 93;
    defaultEPoint.r = 103;
    adeptRobot.moveE(defaultEPoint);    
    getch();
    return 0;
}
