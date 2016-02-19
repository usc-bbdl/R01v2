#include "expParadigm.h"
#include "FPGAControl.h"
#include <servoControl.h>
expParadigm::expParadigm(double offset1,double offset2)
{
    int gD1 = 0, gS1 = 0, gD2 = 0, gS2 = 0;
    int cortex1 = 0, cortex2 =0; 
    currentTrialNum = 0;
    currentRepNum = 0;
    log.loadCellOffset1 = offset1;
    log.loadCellOffset2 = offset2;
    char *configFileName = "spindleTest.txt";
    FILE *configFile;
    char *header[200];
    configFile = fopen("spindleTest.txt","r");
    if (configFile == NULL) {
        printf("Could not open data file");
    }
    fscanf(configFile,"%s\n",&header);
    fscanf(configFile,"%d,%d\n",&numTrials);
    for(int i = 0; i < numTrials; i++){
        fscanf(configFile,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", &gD1, &gS1, &gD2, &gS2,&cortex1,&cortex2, &initPos[i], &finalPos[i], &rampVelocity[i],&trialLength[i], &rep[i]);
        gammaDyn1[i] = gD1;
        gammaSta1[i] = gS1;
        gammaDyn2[i] = gD2;
        gammaSta2[i] = gS2;
        cortexDrive1[i] = cortex1;
        cortexDrive2[i] = cortex2;
    }
    fclose(configFile);
}
int expParadigm::startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController)
{
    int holdPeriod= 1000; 
    bool stayInTheLoop = TRUE;
    char key = 0;
    printf("This experiment has %d trials\n",numTrials);

    bicepFPGA->spindleIaGain = 1.2;
    bicepFPGA->spindleIIGain = 2;
    bicepFPGA->spindleIaOffset = 250;
    bicepFPGA->spindleIIOffset = 50;
    bicepFPGA->spindleIaSynapseGain = 60;
    bicepFPGA->spindleIISynapseGain = 60;
    bicepFPGA->updateParametersFlag = '1';
    Sleep(500);



    tricepFPGA->spindleIaGain = 1.2;
    tricepFPGA->spindleIIGain = 2;
    tricepFPGA->spindleIaOffset = 250;
    tricepFPGA->spindleIIOffset = 50;
    tricepFPGA->spindleIaSynapseGain = 60;
    tricepFPGA->spindleIISynapseGain = 60;
    tricepFPGA->updateParametersFlag = '1';
    Sleep(500);

    //bicepFPGA->spindleIaGain = 0.2;
    //bicepFPGA->spindleIIGain = 0.4;
    //bicepFPGA->spindleIaOffset = 1;
    //bicepFPGA->spindleIIOffset = 1;
    //bicepFPGA->spindleIaSynapseGain = 0;
    //bicepFPGA->spindleIISynapseGain = 0;
    //bicepFPGA->updateParametersFlag = '1';
    //Sleep(500);

    

    //tricepFPGA->spindleIaGain = 0.2;
    //tricepFPGA->spindleIIGain = 0.4;
    //tricepFPGA->spindleIaOffset = 0;
    //tricepFPGA->spindleIIOffset = 0;
    //tricepFPGA->spindleIaSynapseGain = 0;
    //tricepFPGA->spindleIISynapseGain = 0;
    //tricepFPGA->updateParametersFlag = '1';
    //Sleep(500);
    for(int i = 0; i < numTrials && stayInTheLoop == TRUE; i++){ 
        printf("This trial has %d repetitions\t\n\n",rep[i]);
        //printf("Gamma Dynamic is: %2f & Gamma Static is: %2f\n",gammaDyn[i],gammaSta[i]);
        
        Sleep(500);
        
        bicepFPGA->gammaDynamic = gammaDyn1[i];
        bicepFPGA->gammaStatic = gammaSta1[i];
        bicepFPGA->updateGammaFlag = '1';
        Sleep(500);

        bicepFPGA->cortexDrive = cortexDrive1[i];
        bicepFPGA->updateCortexFlag = '1';
        Sleep(500);

        tricepFPGA->gammaDynamic = gammaDyn2[i];
        tricepFPGA->gammaStatic = gammaSta2[i];
        tricepFPGA->updateGammaFlag = '1';
        Sleep(500);

        tricepFPGA->cortexDrive = cortexDrive2[i];
        tricepFPGA->updateCortexFlag = '1';
        Sleep(500);
        
        servo.goDefault();
        Sleep(500);
        realTimeController->resetMuscleLength = TRUE;
        realTimeController->newTrial = 1;
        Sleep(500);
        
        //bicepFPGA->spindleIaGain = 10;
        //bicepFPGA->spindleIIGain = 10;
        //bicepFPGA->spindleIaOffset = -50;
        //bicepFPGA->spindleIIOffset = -500;
        //bicepFPGA->spindleIaSynapseGain = 30;
        //bicepFPGA->spindleIISynapseGain = 0;
        //bicepFPGA->updateParametersFlag = '1';

        //bicepFPGA->spindleIaGain = 15;
        //bicepFPGA->spindleIIGain = 5;
        //bicepFPGA->spindleIaOffset = 0;
        //bicepFPGA->spindleIIOffset = 0;
        //bicepFPGA->spindleIaSynapseGain = 5;
        //bicepFPGA->spindleIISynapseGain = 2;
        //bicepFPGA->updateParametersFlag = '1';

        //tricepFPGA->gammaDynamic = gammaDyn[i];
        //tricepFPGA->gammaStatic = gammaSta[i];
        //tricepFPGA->updateGammaFlag = '1';
        //Sleep(100);
        currentTrialNum = i;
        for (int j = 0; j<rep[i] && stayInTheLoop == TRUE; j++){
            holdPeriod = (trialLength[i]*1000)/2;
            currentRepNum = j;
            printf("Starting trial#  %d and repetition #%d\n\n",i+1,j+1);
            time_t t = time(NULL);
	        tm* timePtr = localtime(&t);
            char fileName[200];
            sprintf_s(
            fileName,
            "C:\\data\\GammaSweep_%4d_%02d_%02d_%02d_%02d_%02d_GammaDyn_%.0f_GammaStat_%.0f_Rep_%d.txt",
            timePtr->tm_year+1900, 
            timePtr->tm_mon+1, 
            timePtr->tm_mday, 
            timePtr->tm_hour, 
            timePtr->tm_min, 
            timePtr->tm_sec,
            gammaDyn1[i],
            gammaSta1[i],
            j+1
            );
            log.fileName = fileName;
            log.trialLength = trialLength[i];
            servo.setPerturbationParameters(initPos[i], finalPos[i], rampVelocity[i], holdPeriod);
            servo.rampHold();
            // Terminate Anytime when Escape Is Pressed...
            if (kbhit()!=0){
                key = getch();
                if (key == 27) { stayInTheLoop = FALSE; }
            }
        }
    }
    if (stayInTheLoop == TRUE) printf("\n Experiment finished...\n");
        else printf("\n\n\n Experiment Terminated...\n\n\n");
    return 0;
}
expParadigm::~expParadigm()
{
}