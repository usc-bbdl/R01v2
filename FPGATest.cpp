#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <utilities.h>
#include <okFrontPanelDLL.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <windows.h>
#include <fstream>
#include <vector>
#include <assert.h>
#include "okFrontPanelDLL.h"
#include <iostream>

#include <windows.h>
#include <functional>
#include <vector>
#include	"NIDAQmx.h"
#include "okFrontPanelDLL.h"
#include <SomeFpga.h>

bool    gAlterDamping = false;


int mainTEST()
{
    char dll_date[32], dll_time[32];

    printf("---- Opal Kelly ---- FPGA-DES Application v1.0 ----\n");
    if (FALSE == okFrontPanelDLL_LoadLib(NULL)) {
        printf("FrontPanel DLL could not be loaded.\n");
    }
    okFrontPanelDLL_GetVersion(dll_date, dll_time);
    printf("FrontPanel DLL loaded.  Built: %s  %s\n", dll_date, dll_time);

    SomeFpga *gXemSpindleBic;
    gXemSpindleBic = new SomeFpga(NUM_NEURON, SAMPLING_RATE, "113700021E"); 

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
                    //gExperimentState = MOTOR_STATE_RUN_PARADIGM;
                    //proceedState(&gExperimentState);
                    break;
                case ' ':       // Move forward in the state machine
                    //proceedState(&gExperimentState);
                    break;
           }
        }
    }
   return 0;
}


