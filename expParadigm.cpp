#include "expParadigm.h"
expParadigm::expParadigm(double offset1,double offset2)
{
    currentTrialNum = 0;
    currentRepNum = 0;
    log.loadCellOffset1 = offset1;
    log.loadCellOffset2 = offset2;
    char *configFileName = "rampnhold.txt";
    FILE *configFile;
    char *header[200];
    configFile = fopen("rampnhold.txt","r");
    if (configFile == NULL) {
        printf("Could not open data file");
    }
    fscanf(configFile,"%s\n",&header);
    fscanf(configFile,"%d\n",&numTrials);
    for(int i = 0; i < numTrials; i++){
        fscanf(configFile,"%lf,%lf,%d,%d\n",&gammaDyn[i],&gammaSta[i],&trialLength[i],&rep[i]);
    }
    fclose(configFile);
}
int expParadigm::startParadigm()
{
    printf("This experiment has %d trials\n",numTrials);
    for(int i = 0; i < numTrials; i++){ 
        printf("This trial has %d repetitions\n",rep[i]+1);
        printf("Gamma Dynamic is: %2f & Gamma Static is: %2f\n",gammaDyn[i],gammaSta[i]);
        //UPDATE NI INFORMATION WITH NEW GAMMA DYNAMIC AND STATIC
        currentTrialNum = i;
        for (int j = 0; j<rep[i]; j++){
            currentRepNum = j;
            printf("Starting trial#  %d and repetition #%d\n",i+1,j+1);
            time_t t = time(NULL);
	        tm* timePtr = localtime(&t);
            char fileName[200];
            sprintf_s(
            fileName,
            "GammaSweep_%4d_%02d_%02d_%02d_%02d_%02d_GammaDyn_%.0f_GammaStat_%.0f_Rep_%d.txt",
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
            //client.sendMessageToServer("PPP");
            log.reset();
            log.startRecording();
            //client.sendMessageToServer("TTT");
        }
    }
    return 0;
}
expParadigm::~expParadigm()
{
}