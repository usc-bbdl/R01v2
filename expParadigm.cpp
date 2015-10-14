#include "expParadigm.h"
#include "FPGAControl.h"
#include <analogClient.h>
#include <servoControl.h>
expParadigm::expParadigm(double offset1,double offset2,analogClient *client)
{
    int gD = 0, gS = 0;
    pClient = client;
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
    fscanf(configFile,"%d\n",&numTrials);
    for(int i = 0; i < numTrials; i++){
        fscanf(configFile,"%d,%d,%d,%d\n", &gD, &gS, &trialLength[i], &rep[i]);
        gammaDyn[i] = gD;
        gammaSta[i] = gS;
    }
    fclose(configFile);
  
}
int expParadigm::startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA)
{
    printf("This experiment has %d trials\n",numTrials);
    for(int i = 0; i < numTrials; i++){ 
        printf("This trial has %d repetitions\n",rep[i]+1);
        printf("Gamma Dynamic is: %2f & Gamma Static is: %2f\n",gammaDyn[i],gammaSta[i]);
        //UPDATE NI INFORMATION WITH NEW GAMMA DYNAMIC AND STATIC
        bicepFPGA->gammaDynamic = gammaDyn[i];
        bicepFPGA->gammaStatic = gammaSta[i];
        bicepFPGA->updateGammaFlag = '1';

        tricepFPGA->gammaDynamic = gammaDyn[i];
        tricepFPGA->gammaStatic = gammaSta[i];
        tricepFPGA->updateGammaFlag = '1';
        Sleep(3000);

        currentTrialNum = i;
        for (int j = 0; j<rep[i]; j++){
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
            gammaDyn[i],
            gammaSta[i],
            j+1
            );
            log.fileName = fileName;
            log.trialLength = trialLength[i];
            //client.sendMessageToServer("RRR");
            Sleep(1000);
            //pClient->sendMessageToServer(MESSAGE_PERTURB);
            servo.rampHold();
            //log.reset();
            //log.startRecording();
            Sleep(3000);
        }
    }
    printf("\n Experiment finished...\n");
    return 0;
}
expParadigm::~expParadigm()
{
}