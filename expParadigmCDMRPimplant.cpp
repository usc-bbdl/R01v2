#include "expParadigmCDMRPimplant.h"
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <math.h>
const int Trials = 32; //replace numTrials with Trials and vice versa
expParadigmCDMRPimplant::expParadigmCDMRPimplant(motorControl *temp)
{
    this->motorObj = temp;
    defaultPoint.x = 89.1;
    defaultPoint.y = -58.2;
    defaultPoint.z = 154.2;
    defaultPoint.a = -78.5;
    defaultPoint.b = -61.4;
    defaultPoint.c = 81.6;
    robotPerturbationLive = false;
    numberOfPerturbations = 0;
    adeptRobot.connectToController();
    numTrials = 0;
    readData();
    perturbationAngle = 0;
}

int expParadigmCDMRPimplant::setPerturbationAngle(double angle)
{
    this->perturbationAngle = angle;
    return 1;
}
expParadigmCDMRPimplant::~expParadigmCDMRPimplant(void)
{
    long double angle[6];
    adeptRobot.move(defaultPoint);
    angle[0] = defaultPoint.x;
    angle[1] = defaultPoint.y;
    angle[2] = defaultPoint.z;
    angle[3] = defaultPoint.a;
    angle[4] = defaultPoint.b;
    angle[5] = defaultPoint.c;
}

void expParadigmCDMRPimplant::sweepAngleForce(double forceMin, double forceMax, double forceResolution, double  angleMin, double angleMax, double angleResolution, int numberOfPerturbations)
{
    for (double force = forceMin; force<forceMax; force = force + forceResolution)
    {
        for (double angle = angleMin; angle<angleMax; angle = angle + angleResolution)
        {
            printf("Force = %3.6f, Angle = %3.6f\n",force,angle);
            this->numberOfPerturbations = numberOfPerturbations;
            setPerturbationAngle(angle);
            motorObj->motorRef[0] = force;
            while (robotPerturbationLive == TRUE)
            {
            }
            temp();
        }
    }
}

int expParadigmCDMRPimplant::temp(void)
{
    robotPerturbationLive = TRUE;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(expParadigmCDMRPimplant::AdeptPerturbationsLoop,0,this);
    return 1;
}
void expParadigmCDMRPimplant::readData()
{
    //numTrials = Trials;    
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
    //numTrials = 32;
    std::cout<<"\n\n NumTrials:"<<numTrials<<"\n\n";
    double tempA=0, tempF=0;
    int tempR=0;

    for(int i = 0; i < numTrials; i++){
        fscanf(configFile,"%lf,%lf,%d\n", &tempA, &tempF, &tempR);
        amp[i] = tempA;
        freq[i] = tempF;
        rep[i] = tempR;
        //printf("\n%lf,%lf,%d\n", tempA, tempF, tempR);
    }
    fclose(configFile);
}
int expParadigmCDMRPimplant::setAdeptDefaultPosition(double * position)
{
    defaultPoint = PPoint(position[0],position[1],position[2],position[3],position[4],position[5]);
    return 1;
}


int expParadigmCDMRPimplant::startAdeptPerturbations(int numberOfPerturbations)
{
    this->numberOfPerturbations = numberOfPerturbations;
    robotPerturbationLive = TRUE;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(expParadigmCDMRPimplant::AdeptPerturbationsLoop,0,this);
    return 1;
}
void expParadigmCDMRPimplant::AdeptPerturbationsLoop(void* a)
{
	((expParadigmCDMRPimplant*)a)->perturbAdept();
}
int expParadigmCDMRPimplant::perturbAdept()
{
//    PPoint defaultPoint, newPoint;
//    defaultPoint = PPoint(169.99,-15.63,80.339,75.465,28.90,11);
    long double angle[6];
    angle[0] = defaultPoint.x;
    angle[1] = defaultPoint.y;
    angle[2] = defaultPoint.z;
    angle[3] = defaultPoint.a;
    angle[4] = defaultPoint.b;
    angle[5] = defaultPoint.c;
    double flexAngle = perturbationAngle; 
    double extendAngle = -perturbationAngle;
    bool isFlex = true;
    //adeptRobot.setVelocity (10, 10, MONITOR, true);
    adeptRobot.move(defaultPoint);
    for (int i = 0; i<numberOfPerturbations; i++)
    {
        printf("i = %d\n",i);
        //adeptRobot.move(defaultPoint);
        angle[0] = defaultPoint.x;
        angle[1] = defaultPoint.y;
        angle[2] = defaultPoint.z;
        angle[3] = defaultPoint.a;
        angle[4] = defaultPoint.b;
        angle[5] = defaultPoint.c;
        if (isFlex==true)
        {
            angle[5] = angle[5] + flexAngle;
            isFlex = false;
            motorObj->perturbationAngle = flexAngle;
        }
        else if (isFlex==false)
        {
            angle[5] = angle[5] + extendAngle;
            isFlex = true;
            motorObj->perturbationAngle = extendAngle;
        }
        motorObj->trialTrigger = 2;
        newPoint = PPoint(angle[0],angle[1],angle[2],angle[3],angle[4],angle[5]);
        adeptRobot.move(newPoint);
        Sleep(2000);
    }
    adeptRobot.move(defaultPoint);
    robotPerturbationLive = FALSE;
    return 1;
}
int expParadigmCDMRPimplant::startParadigm(motorControl *motorObj)
{
    //numTrials = Trials;
        //________________________________________________________________
    motorObj->newPdgm_Flag = true;

    //motorObj->newPdgm_ref[0] = (double)numTrials;
    //motorObj->newPdgm_ref[1] = (double)numTrials;

    int retVal = -1, menu = 0, exitFlag = 0;
    bool stayInTheLoop = TRUE;
    char key = 0;
    printf("\n\CDMRP implant Perturbation Options\n");
        printf("\t[0] Exit CDMRP implant Perturbation\n"); 

        printf("This CDMRP experiment has %d trials\n",numTrials);
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
        int forceOffset = 2;
        double sinVal = 0;
        double c = 6;
        for(int i=0; i<numTrials && stayInTheLoop; i++)
        {
            
            printf("This trial has %d repetitions\t\n\n",rep[i]);
            double f = freq[i];
            double A = amp[i];
            
            for(int j=0; j<rep[i] && stayInTheLoop; j++)
            {
                printf("Starting trial#  %d and repetition #%d. AMP = %lf, FRQ = %lf\n\n",i+1,j+1, A, f);
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
                    f0 = (0.5*(0-sinVal)) + forceOffset;
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