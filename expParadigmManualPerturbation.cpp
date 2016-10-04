#include "expParadigmManualPerturbation.h"
#include "FPGAControl.h"
#include <conio.h>
#include <stdio.h>

expParadigmManualPerturbation::expParadigmManualPerturbation()
{
}
int expParadigmManualPerturbation::startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController)
{
    int retVal = -1, menu = 0, exitFlag = 0, updateIT = 1;
    bool stayInTheLoop = TRUE;
    char key = 0;

    bicepFPGA->spindleIaGain = 1.2;
    bicepFPGA->spindleIIGain = 2;
    bicepFPGA->spindleIaOffset = 250;
    bicepFPGA->spindleIIOffset = 50;
    bicepFPGA->spindleIaSynapseGain = 60;
    bicepFPGA->spindleIISynapseGain = 60;
    bicepFPGA->forceLengthCurve = 1; //default 1
    bicepFPGA->updateParametersFlag = '1';
    Sleep(500);



    tricepFPGA->spindleIaGain = 1.2;
    tricepFPGA->spindleIIGain = 2;
    tricepFPGA->spindleIaOffset = 250;
    tricepFPGA->spindleIIOffset = 50;
    tricepFPGA->spindleIaSynapseGain = 60;
    tricepFPGA->spindleIISynapseGain = 60;
    tricepFPGA->forceLengthCurve = 1; //default 1
    tricepFPGA->updateParametersFlag = '1';
    Sleep(500);

  
    do{
        if (updateIT == 1) {
            bicepFPGA->gammaDynamic = gammaDyn1;
            bicepFPGA->gammaStatic = gammaSta1;
            bicepFPGA->updateGammaFlag = '1';
            Sleep(500);

            bicepFPGA->cortexDrive = cortexDrive1;
            bicepFPGA->updateCortexFlag = '1';
            Sleep(500);

            tricepFPGA->gammaDynamic = gammaDyn2;
            tricepFPGA->gammaStatic = gammaSta2;
            tricepFPGA->updateGammaFlag = '1';
            Sleep(500);

            tricepFPGA->cortexDrive = cortexDrive2;
            tricepFPGA->updateCortexFlag = '1';
            Sleep(500);
            //realTimeController->resetMuscleLength = TRUE;
            realTimeController->trialTrigger = 1;
            Sleep(500);       
            updateIT = 0;
        }//end update if block

        printf("\n\nManual Perturbation Options\n");
        printf("\t[0] Exit Manual Paradigm\n"); 
        printf("\t[1] Change Muscle 1 Parameters\n");
        printf("\t[2] Change Muscle 2 Parameters\n");
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
            printf("\tMuscle 1 - Cortex Drive:\n");
            std::cin>>cortexDrive1;
            updateIT = 1;
            printf("\tMuscle 1 -Gamma Static: %f -Gamma Dynamic: %f -Cortex Drive: %f\n", gammaSta1, gammaDyn1, cortexDrive1);
            break;
        case 2:
            printf("\tMuscle 2 - Gamma Static:\n");
            std::cin>>gammaSta2;
            printf("\tMuscle 2 - Gamma Dynamic:\n");
            std::cin>>gammaDyn2;
            printf("\tMuscle 2 - Cortex Drive:\n");
            std::cin>>cortexDrive2;
            updateIT = 1;
            printf("\tMuscle 2 -Gamma Static: %f -Gamma Dynamic: %f -Cortex Drive: %f\n", gammaSta2, gammaDyn2, cortexDrive2);
            break;
        case 0:   
            exitFlag = 1;
            break;
        default: break;
        }//end input switch        
    }while(exitFlag == 0);

    return 1;
}


expParadigmManualPerturbation::~expParadigmManualPerturbation()
{
}
