// experimentUserInterface.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <utilities.h>
#include <okFrontPanelDLL.h>
#include <servoControl.h>
/*
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
   return 0;
}
*/