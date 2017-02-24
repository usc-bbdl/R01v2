#include "expParadigmCDMRPimplant.h"
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <math.h>

expParadigmCDMRPimplant::expParadigmCDMRPimplant(void)
{
}


expParadigmCDMRPimplant::~expParadigmCDMRPimplant(void)
{
}

void expParadigmCDMRPimplant::readData()
{
    numTrials = 0;
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
    std::cout<<"\n\n NumTrials:"<<numTrials<<"\n\n";
    double tempA=0, tempF=0;
    int tempR=0;

    for(int i = 0; i < numTrials; i++){
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

        //________________________________________________________________
    motorObj->newPdgm_Flag = true;

    //motorObj->newPdgm_ref[0] = (double)numTrials;
    //motorObj->newPdgm_ref[1] = (double)numTrials;

    int retVal = -1, menu = 0, exitFlag = 0;
    bool stayInTheLoop = TRUE;
    char key = 0;
    printf("\n\CDMRP implant Perturbation Options\n");
        printf("\t[0] Exit CDMRP implant Perturbation\n"); 

        printf("This experiment has %d trials\n",numTrials);
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

        double c = 6;
        for(int i=0; i<numTrials && stayInTheLoop; i++)
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
                double f0=0, f1=0;

                do
                {
                tock = motorObj->getTime();
                f0 = 0;
                f1 = A*sin(2*PI*f*(tock-tick));
                //if( ((tock-tick)>=(rising*period)) && ((tock-tick)<=(falling*period)) ) f0 = c;
                //if( ((tock-tick)<(phase_shift*period)) ) f1 = 0;

                if(f1<0) 
                {
                    f1=0;
                    f0 = c;
                }
                else f0 = 0;

                motorObj->newPdgm_ref[0] = f0;
                motorObj->newPdgm_ref[1] = f1;
                
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
            motorObj->newPdgm_ref[1] = 0;
            motorObj->newPdgm_ref[0] = c;
            Sleep(1000);
            //getch();
        }
        
                //getch();
    motorObj->newPdgm_Flag = false;
    //motorObj->newPdgm_ref[0] = 5;
    //motorObj->newPdgm_ref[1] = 5;
    return 1;
}