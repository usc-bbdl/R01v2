#include "expParadigmServoPerturbation.h"
#include "FPGAControl.h"
#include "servoControl.h"
expParadigmServoPerturbation::expParadigmServoPerturbation(servoControl *param)
{
    servo = param;
    int gD1 = 0, gS1 = 0, gD2 = 0, gS2 = 0;
    int cortex1 = 0, cortex2 =0, inPos = 0, finPos = 0,rampVel = 0, trialLen = 0, r = 0;
    currentTrialNum = 0;
    currentRepNum = 0;
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
        fscanf(configFile,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", &gD1, &gS1, &gD2, &gS2, &cortex1, &cortex2, &inPos, &finPos, &rampVel, &trialLen, &r);
        gammaDyn1[i] = gD1;
        gammaSta1[i] = gS1;
        gammaDyn2[i] = gD2;
        gammaSta2[i] = gS2;
        cortexDrive1[i] = cortex1;
        cortexDrive2[i] = cortex2;
        initPos[i] = inPos;
        finalPos[i] = finPos;
        rampVelocity[i] = rampVel;
        trialLength[i] = trialLen;
        rep[i] = r;
    }
    fclose(configFile);
}
int expParadigmServoPerturbation::startParadigm(FPGAControl *bicepFPGA, FPGAControl *tricepFPGA, motorControl *realTimeController)
{
    int holdPeriod= 1000, retVal = -1;
    bool stayInTheLoop = TRUE;
    char key = 0;
    printf("This experiment has %d trials\n",numTrials);
    for(int i = 0; i < numTrials && stayInTheLoop == TRUE; i++){
        printf("This trial has %d repetitions\t\n\n",rep[i]);
        //printf("Gamma Dynamic is: %2f & Gamma Static is: %2f\n",gammaDyn[i],gammaSta[i]);

        Sleep(100);

        bicepFPGA->gammaDynamic = gammaDyn1[i];
        bicepFPGA->gammaStatic = gammaSta1[i];
        bicepFPGA->updateGammaFlag = '1';
        Sleep(500);

        //bicepFPGA->cortexDrive = cortexDrive1[i];
        //bicepFPGA->updateCortexFlag = '1';
        //Sleep(500);

        tricepFPGA->gammaDynamic = gammaDyn2[i];
        tricepFPGA->gammaStatic = gammaSta2[i];
        tricepFPGA->updateGammaFlag = '1';
        Sleep(500);

        //tricepFPGA->cortexDrive = cortexDrive2[i];
        //tricepFPGA->updateCortexFlag = '1';
        //Sleep(500);

        servo->goDefault();
        servo->waitMoving();
        Sleep(100);

        realTimeController->resetLength();
        realTimeController->newTrial(1);
        realTimeController->setPerturbationAngle((initPos[i] + finalPos[i]) /2);
        realTimeController->setPerturbationVelocity(rampVelocity[i]);

        Sleep(100);
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
            realTimeController->newTrial(1);
            servo->setPerturbationParameters(initPos[i], finalPos[i], rampVelocity[i], holdPeriod);
            servo->setVelocity(rampVelocity[i]);
            servo->setPosition(initPos[i]);
            servo->waitMoving();
            Sleep(holdPeriod);

            realTimeController->newTrial(3);
            servo->setPosition(finalPos[i]);
            servo->waitMoving();
            Sleep(holdPeriod);    

            // Terminate Anytime when Escape Is Pressed...
            if (kbhit()!=0){
                key = getch();
                if (key == 27) {
                    stayInTheLoop = FALSE;
                    retVal = -1;
                }
                else if (key == 'q' || key == 'Q') {
                    stayInTheLoop = FALSE;
                    retVal = 1;
                }
            }
        }
    }


    if (stayInTheLoop == TRUE) {
        printf("\n Experiment finished...\n");
        return 0;
    }
        else {
            printf("\n\n\n Experiment Terminated...\n\n\n");
            return retVal;
        }
    return 0;
}


expParadigmServoPerturbation::~expParadigmServoPerturbation()
{
    int i;
}