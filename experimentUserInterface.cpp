// experimentUserInterface.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <utilities.h>
#include <okFrontPanelDLL.h>

int main()
{
    printf("Press Spc to move forward\n\n");
    printf("Press Esc to terminate\n");
    printf("Initialization; Next stage is Motors Winding up\n");

    char dll_date[32], dll_time[32];

    printf("---- Opal Kelly ---- FPGA-DES Application v1.0 ----\n");
    if (FALSE == okFrontPanelDLL_LoadLib(NULL)) {
        printf("FrontPanel DLL could not be loaded.\n");
    }
    okFrontPanelDLL_GetVersion(dll_date, dll_time);
    printf("FrontPanel DLL loaded.  Built: %s  %s\n", dll_date, dll_time);


    int gExperimentState = MOTOR_STATE_INIT;
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
                    gExperimentState = MOTOR_STATE_RUN_PARADIGM;
                    proceedState(&gExperimentState);
                    break;
                case ' ':       // Move forward in the state machine
                    proceedState(&gExperimentState);
                    break;
           }
        }
    }
   return 0;
}

