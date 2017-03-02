#include "expParadigmCDMRPimplant.h"
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <math.h>
const int Trials = 32; //replace numTrials with Trials and vice versa
expParadigmCDMRPimplant::expParadigmCDMRPimplant(void)
{
    numTrials = 0;
}


expParadigmCDMRPimplant::~expParadigmCDMRPimplant(void)
{
}

void expParadigmCDMRPimplant::readData()
{
    numTrials = Trials;    
    //C:\Users\PXI_BBDL\Documents\GitHub\R01v2
    char *configFileName = "CDMRPtest.txt";
    FILE *configFile;
    char header[200];
    configFile = fopen("C:\\Users\\PXI_BBDL\\Documents\\GitHub\\R01v2\\CDMRPtest.txt","r");
    if (configFile == NULL) {
        printf("\n\nCould not open data file\n\n");
    }
    fscanf(configFile,"%s\n",&header);
    std::cout<<"\n\n NumTrials:"<<header<<"\n\n";
    fscanf(configFile,"%d\n",&numTrials);
    numTrials = 32;
    std::cout<<"\n\n NumTrials:"<<numTrials<<"\n\n";
    double tempA=0, tempF=0;
    int tempR=0;

    for(int i = 0; i < Trials; i++){
        fscanf(configFile,"%lf,%lf,%d\n", &tempA, &tempF, &tempR);
        amp[i] = tempA;
        freq[i] = tempF;
        rep[i] = tempR;
        printf("\n%lf,%lf,%d\n", tempA, tempF, tempR);
    }
    fclose(configFile);
}

int expParadigmCDMRPimplant::startParadigm(motorControl *motorObj)
{
    numTrials = Trials;
        //________________________________________________________________
    motorObj->newPdgm_Flag = true;

    //motorObj->newPdgm_ref[0] = (double)numTrials;
    //motorObj->newPdgm_ref[1] = (double)numTrials;

    int retVal = -1, menu = 0, exitFlag = 0;
    bool stayInTheLoop = TRUE;
    char key = 0;
    printf("\n\CDMRP implant Perturbation Options\n");
        printf("\t[0] Exit CDMRP implant Perturbation\n"); 

        printf("This CDMRP experiment has %d trials\n",Trials);
        /*
        do{
        do{
            scanf("%d", &menu);
            if (!((menu <= 0) || (menu >= 0)))
                printf("\n\nWrong input! try Again.\n\n");
        }while (!((menu <= 0) || (menu >= 0)));
        switch(menu)
        {
        case 0: exitFlag = 1; break;

        default: break;
        }
    }while(exitFlag == 0);
    */
        //motorObj->newPdgm_ref[0] = numTrials;
        //        motorObj->newPdgm_ref[1] = numTrials;
        int forceOffset = 2;\
        double sinVal = 0;
        double c = 6;
        for(int i=0; i<Trials && stayInTheLoop; i++)
        {
            
            printf("This trial has %d repetitions\t\n\n",rep[i]);
            double f = freq[i];
            double A = amp[i];
            
            for(int j=0; j<rep[i] && stayInTheLoop; j++)
            {
                printf("Starting trial#  %d and repetition #%d\n\n",i+1,j+1);
                double tick = motorObj->getTime();
                double tock;
                double period = 1/f;
                double phase_shift = 0.5;
                double rising = 0.05;
                double falling = 0.3;
                
                falling = falling + rising;
                double f0=forceOffset, f1=forceOffset;

                do
                {
                tock = motorObj->getTime();
                sinVal = A*sin(2*PI*f*(tock-tick));
                f0 = forceOffset;
                f1 = sinVal + forceOffset;
                //if( ((tock-tick)>=(rising*period)) && ((tock-tick)<=(falling*period)) ) f0 = c;
                //if( ((tock-tick)<(phase_shift*period)) ) f1 = 0;

                if(sinVal < 0) 
                {
                    f1=forceOffset;
                    f0 = 0.5*((0-sinVal) + forceOffset);
                }
                //f0 extensor
                //f1 flexor
                else f0 = forceOffset;

                motorObj->newPdgm_ref[0] = f0;
                motorObj->newPdgm_ref[1] = f1;
                Sleep(10);
                //printf("\nF0: %lf, F1: %lf\n\n",f0,f1);

                            if (kbhit()!=0)
                            {
                                    key = getch();
                                    if (key == 27) 
                                    {
                                        stayInTheLoop = FALSE;
                                        retVal = -1;
                                    }
                                    else if (key == 'q' || key == 'Q') 
                                    {
                                        stayInTheLoop = FALSE;
                                        retVal = 1;
                                    }
                            }

                }while((tock-tick)<period);
               
            } 
            motorObj->newPdgm_ref[1] = forceOffset;
            motorObj->newPdgm_ref[0] = forceOffset;
            Sleep(1000);
            //getch();
        }
        
                //getch();
    motorObj->newPdgm_Flag = false;
    //motorObj->newPdgm_ref[0] = 5;
    //motorObj->newPdgm_ref[1] = 5;
    return 1;
}