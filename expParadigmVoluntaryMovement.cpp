#include "expParadigmVoluntaryMovement.h"
#include "FPGAControl.h"
#include <conio.h>
#include <stdio.h>
#include <utilities.h>

expParadigmVoluntaryMovement::expParadigmVoluntaryMovement(motorControl *mtr)
{
    vfTemp = false;
    cortexAmp  = &(mtr->cortexVoluntaryAmp);
    cortexFreq = &(mtr->cortexVoluntaryFreq);
}
int expParadigmVoluntaryMovement::startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController)
{
    int retVal = -1, menu = 0, exitFlag = 0, updateIT = 1;
    bool stayInTheLoop = TRUE;
    double temp = 0;
    char key = 0;
    vfTemp = dataAcquisitionFlag[11];

    bicepFPGA->spindleIaGain = 1.2;
    bicepFPGA->spindleIIGain = 2;
    bicepFPGA->spindleIaOffset = 250;
    bicepFPGA->spindleIIOffset = 50;
    bicepFPGA->spindleIaSynapseGain = 60;
    bicepFPGA->spindleIISynapseGain = 60;
    bicepFPGA->forceLengthCurve = 1;
    bicepFPGA->updateParametersFlag = '1';
    Sleep(500);



    tricepFPGA->spindleIaGain = 1.2;
    tricepFPGA->spindleIIGain = 2;
    tricepFPGA->spindleIaOffset = 250;
    tricepFPGA->spindleIIOffset = 50;
    tricepFPGA->spindleIaSynapseGain = 60;
    tricepFPGA->spindleIISynapseGain = 60;
    tricepFPGA->forceLengthCurve = 1;
    tricepFPGA->updateParametersFlag = '1';
    Sleep(500);

    dataAcquisitionFlag[11] = true;
    do{
         if (updateIT == 1) {
            bicepFPGA->gammaDynamic = gammaDyn1;
            bicepFPGA->gammaStatic = gammaSta1;
            bicepFPGA->updateGammaFlag = '1';
            Sleep(500);

            tricepFPGA->gammaDynamic = gammaDyn2;
            tricepFPGA->gammaStatic = gammaSta2;
            tricepFPGA->updateGammaFlag = '1';
            Sleep(500);

            //realTimeController->resetMuscleLength = TRUE;
            realTimeController->trialTrigger = 1;
            Sleep(500);       
            updateIT = 0;
        }//end update if block

        printf("\n\nVOluntary Movement Options\n");
        printf("\t[0] Exit Voluntary Movement Paradigm\n"); 
        printf("\t[1] Change Muscle 1 Parameters\n");
        printf("\t[2] Change Muscle 2 Parameters\n");
        printf("\t[3] Change Voluntary Drive\n");
        do{
            scanf("%d", &menu);
            if (!((menu <= 3) || (menu >= 0)))
                printf("\n\nWrong input! try Again.\n\n");
        }while (!((menu <= 3) || (menu >= 0)));
        switch(menu)
        {
        case 1:
            printf("\tMuscle 1 - Gamma Static:\n");
            std::cin>>gammaSta1;
            printf("\tMuscle 1 - Gamma Dynamic:\n");
            std::cin>>gammaDyn1;
            updateIT = 1;
            printf("\tMuscle 1 -Gamma Static: %f -Gamma Dynamic: %f\n", gammaSta1, gammaDyn1);
            break;
        case 2:
            printf("\tMuscle 2 - Gamma Static:\n");
            std::cin>>gammaSta2;
            printf("\tMuscle 2 - Gamma Dynamic:\n");
            std::cin>>gammaDyn2;
            updateIT = 1;
            printf("\tMuscle 2 -Gamma Static: %f -Gamma Dynamic: %f\n", gammaSta2, gammaDyn2);
            break;
        case 3:
            printf("\tVoluntary Drive - Cortex Amplitude:\n");
            std::cin>>temp;
            *cortexAmp = temp;
            printf("\tVoluntary Drive - Cortex Frequency:\n");
            std::cin>>temp;
            *cortexFreq = temp;
            printf("\tVoluntary Drive -Cortex Amplitude: %f -Cortex Frequency: %f\n", *cortexAmp, *cortexFreq);
            break;
        case 0:   
            exitFlag = 1;
            break;
        default: break;
        }//end input switch       
    }while(exitFlag == 0);

    dataAcquisitionFlag[11] = vfTemp;
    return 1;
}


expParadigmVoluntaryMovement::~expParadigmVoluntaryMovement()
{
}
