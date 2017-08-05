#include "expParadigmCDMRPimplant.h"
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <math.h>
const int Trials = 32; //replace numTrials with Trials and vice versa
expParadigmCDMRPimplant::expParadigmCDMRPimplant(motorControl *temp)
{
    this->motorObj = temp;
    double defaultPos[6] = {158.4,-62.3,160.2,-87.7,-80.6,-3.4};
    setAdeptDefaultPosition(defaultPos);
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
    this->numberOfPerturbations = numberOfPerturbations;
    for (double force = forceMin; force<forceMax; force = force + forceResolution)
    {
        for (double angle = angleMin; angle<angleMax; angle = angle + angleResolution)
        {
            while (robotPerturbationLive == TRUE)
            {
            }
            printf("Force = %3.6f, Angle = %3.6f\n",force,angle);
            setPerturbationAngle(angle);
            motorObj->motorRef[0] = force;
            beginRobotPertThread();
        }
    }
}

int expParadigmCDMRPimplant::beginRobotPertThread(void)
{
    robotPerturbationLive = TRUE;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(expParadigmCDMRPimplant::AdeptPerturbationsLoop,0,this);
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
        printf("perturbation %d / %d\r",i+1,numberOfPerturbations);
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
    Sleep(2000);
    robotPerturbationLive = FALSE;
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