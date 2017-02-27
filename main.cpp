// experimentUserInterface.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include "utilities.h"

#include "dataOneSample.h"
#include "motorControl.h"
#include "expParadigmMuscleLengthCalibration.h"

#include "motorControl.h"
#include <math.h>
#include <algorithm>

#include "NIDAQmx.h"

//#include <okFrontPanelDLL.h>
//#include <servoControl.h>
/*
    float64 loadCellData[2],motorRef[2],muscleLength[2],muscleVel[2];
    int32       error=0;
    TaskHandle  motorTaskHandle, motorEnableHandle,loadCelltaskHandle;
    TaskHandle  encodertaskHandle[2];

    encoderData1[0] = 0;
    encoderData2[0] = 0;


    bool32 isLate = {0};
    */

/*
int main()
{
    printf("Press 'Spc' to move forward\n\n");
    printf("Press 'Esc' to terminate\n");
    printf("\nInitialization; Next stage is Motors Winding up\n");
    //char dll_date[32], dll_time[32];

    static servoControl servo;
    static dataOneSample loadCellOffsets;
    static motorControl motors(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    static expParadigmMuscleLengthCalibration paradigmMuscleLengthCalibration(&servo);

    printf("Motors Winding Up; Next stage is Open-Loop\n");
        motors.motorEnable();
        motors.motorWindUp();

    motors.motorControllerStart();
        Sleep(1000);
        motors.resetMuscleLength = TRUE;

    int retVal = paradigmMuscleLengthCalibration.startParadigm(&motors);
    if(retVal != -1){}
            //*state = STATE_CLOSED_LOOP;
            else {
                //*state = STATE_SHUTTING_DOWN;
                printf("\nPress space to shutdown\n");
            }

            
            motors.dummy();
            

    /*
    //originally commented
    printf("\n---- Opal Kelly ---- FrontPanel 3.0 ----\n");
    if (FALSE == okFrontPanelDLL_LoadLib(NULL)) {
        printf("FrontPanel DLL could not be loaded.\n");
    }
    okFrontPanelDLL_GetVersion(dll_date, dll_time);
    printf("FrontPanel DLL loaded.  Built: %s  %s\n", dll_date, dll_time);
    

    int gExperimentState = STATE_INIT;
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
                    gExperimentState = STATE_SHUTTING_DOWN;
                    proceedState(&gExperimentState);
                    break;
                case ' ':       // Move forward in the state machine
                    proceedState(&gExperimentState);
                    break;
           }
        }
    }

    */
   //return 0;
//}

