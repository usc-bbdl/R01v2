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
    vtick = &(mtr->tick);
    vTempTick = &(mtr->tempTick);
    vFREQ  = &(mtr->mFREQ);
    vCortexA = &(mtr->mCortexA);
    vGammaSA  = &(mtr->mGammaSA);
    vGammaSP = &(mtr->mGammaSP);
    vGammaDA  = &(mtr->mGammaDA);
    vGammaDP = &(mtr->mGammaDP);
    vreps  = &(mtr->mreps);
    vFlag = &(mtr->mFlag);

    configFileName = "voluntaryTest.txt";
    FILE *configFile;
    char *header[200];
    configFile = fopen(configFileName,"r");
    if (configFile == NULL) {
        printf("Could not open data file");
    }
    fscanf(configFile,"%s\n",&header);
    fscanf(configFile,"%d\n",&numTrials);

    printf("\n\nPrinting paradigm file: %s, %lu\n\n",header,numTrials);

    FREQ        = new double[numTrials]; 
    CortexA     = new double[numTrials];
    GammaSA     = new double[numTrials];
    GammaSP     = new double[numTrials];
    GammaDA     = new double[numTrials];
    GammaDP     = new double[numTrials];
    reps        = new unsigned int[numTrials];

    double tFREQ    = 0.0;
    double tCortexA = 0.0;
    double tGammaSA = 0.0;
    double tGammaSP = 0.0;
    double tGammaDA = 0.0;
    double tGammaDP = 0.0;
    unsigned int treps = 0;
    for(unsigned long i = 0; i < numTrials; i++){
        fscanf(configFile,"%lf,%lf,%lf,%lf,%lf,%lf,%d\n",&tFREQ,&tCortexA,&tGammaSA,&tGammaSP,&tGammaDA,&tGammaDP,&treps);
        //printf("\n%f,%f,%f,%f,%f,%f,%d\n",tFREQ,tCortexA,tGammaSA,tGammaSP,tGammaDA,tGammaDP,treps);
        FREQ[i] = tFREQ;
        CortexA[i] = tCortexA;
        GammaSA[i] = tGammaSA;
        GammaSP[i] = tGammaSP;
        GammaDA[i] = tGammaDA;
        GammaDP[i] = tGammaDP;
        reps[i] = treps;
        //printf("\n%f,%f,%f,%f,%f,%f,%d\n",FREQ[i],CortexA[i],GammaSA[i],tGammaSP,tGammaDA,tGammaDP,treps);
    }

    // What trial to start with to begin with
    startTrial = 0;
    printf("\n\nFrom what trial should we begin? [1 ~ %lu]:\n\n", numTrials);
    do{
            scanf("%d", &startTrial);
            if ((startTrial > numTrials) || (startTrial < 1))
                printf("Wrong input! try Again.\n");
        }while ((startTrial > numTrials) || (startTrial < 1));
        
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

    //*vTempTick = *vtick + ( (1/(*vFREQ)) * (reps[0]) );
    dataAcquisitionFlag[11] = true;
    
    unsigned long fileLine = startTrial-1;
    printf("\n\n\nStarting Voluntary Paradigm:Testing %lu Trials [%lu ~ %lu]\n\n",startTrial, numTrials);
    while(fileLine<=numTrials)
    {
        //std::cout<<"\n\n"<<fileLine<<"\t"<<*vFlag<<"\n";
         if (*vFlag == 1) {
             if(fileLine==numTrials)
             {
                 printf("\n\nVoluntary Paradigm Complete! \n\t Press Space to continue...\n\n");
             }
             else{
                *vFREQ  = FREQ[fileLine];
                *vCortexA = CortexA[fileLine];
                *vGammaSA  = GammaSA[fileLine];
                *vGammaSP = GammaSP[fileLine];
                *vGammaDA  = GammaDA[fileLine];
                *vGammaDP = GammaDP[fileLine];
                *vreps  = reps[fileLine];
                *vFlag = 0;
                *vTempTick = *vtick + ( (1/(*vFREQ)) * (*vreps) );
            
                std::cout<<"\n\n\nVoluntary: Trial "<<(fileLine + 1)<<"/"<<numTrials<<std::endl;
                printf("\tSignal Frequency :: %+6.2f\n",*vFREQ);
                printf("\tCortical Drive   :: A: %+6.2f\n",*vCortexA);
                printf("\tGamma Static     :: A: %+6.2f  P: %+6.2f\n",*vGammaSA,*vGammaSP);
                printf("\tGamma Dynamic    :: A: %+6.2f  P: %+6.2f\n\n",*vGammaDA,*vGammaDP);
            }
            fileLine++;
        }//end update if block
    }  

    dataAcquisitionFlag[11] = vfTemp;
    
    return 1;
}


expParadigmVoluntaryMovement::~expParadigmVoluntaryMovement()
{
    delete FREQ, CortexA, GammaSA, GammaSP, GammaDA, GammaDP, reps;
}