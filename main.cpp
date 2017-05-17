// experimentUserInterface.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>

#include "utilities.h"
#include "okFrontPanelDLL.h"
#include "servoControl.h"
#include "motorControl.h"
#include "expParadigmMuscleLengthCalibration.h"
#include "expParadigmServoPerturbation.h"
#include "expParadigmManualPerturbation.h"
#include "expParadigmVoluntaryMovement.h"

int main()
{
    printf("Press 'Spc' to move forward\n\n");
    printf("Press 'Esc' to terminate\n");
    printf("\nInitialization; Next stage is Motors Winding up\n");
    char dll_date[32], dll_time[32];
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
    getch();
   return 0;
}

int proceedState(int *state)
{
    int retVal = 0;
    int menu = 0;
    static servoControl servo;
    servo.goDefault();
    Sleep(200);
    static motorControl motors;
    static expParadigmMuscleLengthCalibration paradigmMuscleLengthCalibration(&servo);
    static expParadigmServoPerturbation paradigmServoPerturbation(&servo);
    static expParadigmManualPerturbation paradigmManualPerturbation;
    static expParadigmVoluntaryMovement paradigmVoluntaryMovement(&motors);
    static FPGAControl bicepFPGA(BICEP,&motors);
    static FPGAControl tricepFPGA(TRICEP,&motors);

    switch(*state)
    {
    case STATE_INIT:
        printf("Motors Winding Up; Next stage is Open-Loop\n");
        motors.motorEnable();
        motors.motorWindUp();
        *state = STATE_WINDING_UP;
        break;
    case STATE_WINDING_UP:
        //Start Neural FPGA and Feeding muscle length data
        *state = STATE_OPEN_LOOP;
        //CONNECT FPGA HERE
        printf("Open-Loop ; Next stage is Closed-Loop\n");
        break;
    case STATE_OPEN_LOOP:
        //Connect the Neural FPGA and DAQ, Start controlling muscle force
        motors.motorControllerStart();
        Sleep(500);
        //motors.mData->resetMuscleLength = TRUE;
        printf("Closed-Loop ; Next stage is Experiment Paradigm\n");
        *state = STATE_CLOSED_LOOP;
        break;
    case STATE_CLOSED_LOOP:        
        printf("\n\nWhat Paradigm do you want to run?\n");
        printf("\t[0] Exit Program\n");
        printf("\t[1] Muscle Length Calibration\n");
        printf("\t[2] Servo Perturbation\n");
        printf("\t[3] Manual Perturbation\n");
        printf("\t[4] Voluntary Movement\n\n User Input:");
        do{
            scanf("%d", &menu);
            //printf("catch something\n");

            if (!((menu <= 4) || (menu >= 0))){
                printf("Wrong input! try Again.\n");
            }else{
                //printf("right input! try Again.\n");
            }

        }while (!((menu <= 4) || (menu >= 0)));

        switch(menu)
        {
        case 1:
            *state = STATE_PARADIGM_LENGTH_CALIBRATION;
            printf("Muscle Length Calibration Selected\n");
            printf("Press Space to continue\n");
            break;
        case 2:
            *state = STATE_RUN_PARADIGM_SERVO_PERTURBATION;
            printf("Servo Perturbation Selected\n");
            printf("Press Space to continue\n");
            break;
        case 3:
            *state = STATE_RUN_PARADIGM_MANUAL_PERTURBATION;
            printf("Manual Perturbation Selected\n");
            printf("Press Space to continue\n");
            break;
        case 4:
            *state = STATE_RUN_PARADIGM_VOLUNTARY_MOVEMENT;
            printf("Voluntary Movement Selected\n");
            printf("Press Space to continue\n");
            break;
        case 0:
            *state = STATE_SHUTTING_DOWN;
            printf("\nPress space to shutdown\n");
            printf("Press Space to continue\n");
            break;
        default: break;
        }
//        Sleep(500);
//        paradigm.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        break;
    case STATE_PARADIGM_LENGTH_CALIBRATION:
        retVal = paradigmMuscleLengthCalibration.startParadigm(&motors);
        if(retVal != -1)
            *state = STATE_CLOSED_LOOP;
            else {
                *state = STATE_SHUTTING_DOWN;
                printf("\nPress space to shutdown\n");
            }
        break;
    case STATE_RUN_PARADIGM_SERVO_PERTURBATION:
        retVal = paradigmServoPerturbation.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        if(retVal != -1)
            *state = STATE_CLOSED_LOOP;
            else {
                *state = STATE_SHUTTING_DOWN;
                printf("\nPress space to shutdown\n");
            }
        break;
    case STATE_RUN_PARADIGM_MANUAL_PERTURBATION:
        retVal = paradigmManualPerturbation.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        if(retVal != -1)
            *state = STATE_CLOSED_LOOP;
            else {
                *state = STATE_SHUTTING_DOWN;
                printf("\nPress space to shutdown\n");
            }
        break;
    case STATE_RUN_PARADIGM_VOLUNTARY_MOVEMENT:
        retVal = paradigmVoluntaryMovement.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        if(retVal != -1)
            *state = STATE_CLOSED_LOOP;
            else {
                *state = STATE_SHUTTING_DOWN;
                printf("\nPress space to shutdown\n");
            }
        break;
    case STATE_SHUTTING_DOWN:
        printf("Shutting Down\n");
        motors.motorControllerEnd();
        printf("Press Enter to Exit\n");
        exit(0);
        break;
    }
    return 0;
}
void waitkey() {
	getchar();
}