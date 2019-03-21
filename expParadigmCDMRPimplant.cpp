#include "expParadigmCDMRPimplant.h"
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <math.h>
#define shellSleep 2000

const int Trials = 32; //replace numTrials with Trials and vice versa

expParadigmCDMRPimplant::expParadigmCDMRPimplant(motorControl *temp)
{
    this->motorObj = temp;
     defaultPos[0] =  157.202; // x
     defaultPos[1] =  -61.430; // y
     defaultPos[2] =  155.096; // z
     defaultPos[3] =  -3.946; // a
     defaultPos[4] =  -11.830; // b
     defaultPos[5] =   -180.029; // c
     /*
     defaultPos[0] =  157.202; // J1
     defaultPos[1] =  -61.430; // J2
     defaultPos[2] =  155.096; // J3
     defaultPos[3] =  -3.946; // J4
     defaultPos[4] =  -11.830; // J5
     defaultPos[5] =   -180.029; // J6
     */

    //double defaultPos[6] = {154.548,-62.378,158.209,-92.896,-64.234,13.145};

    setAdeptDefaultPosition();
    robotPerturbationLive = false;
    numberOfPerturbations = 0;
    adeptRobot.connectToController();
    numTrials = numPerts = 0;
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
    adeptRobot.movetrans(defaultPoint);
    angle[0] = defaultPoint.x;
    angle[1] = defaultPoint.y;
    angle[2] = defaultPoint.z;
    angle[3] = defaultPoint.a;
    angle[4] = defaultPoint.b;
    angle[5] = defaultPoint.c;
}

void expParadigmCDMRPimplant::sweepAngleForce(double forceMin, double forceMax, double forceResolution, double  angleMin, double angleMax, double angleResolution, int numberOfPerturbations)
{   
    double force, angle;
    this->numberOfPerturbations = numberOfPerturbations;
    for (force = forceMin; force <= forceMax; force = force + forceResolution)
    {
        for (angle = angleMin; angle <= angleMax; angle = angle + angleResolution)
        {
            while (robotPerturbationLive == TRUE)
            {
            }
            printf("Force = %3.2f, Angle = %3.2f\n",force,angle);
            setPerturbationAngle(angle);
            motorObj->motorRef[0] = force;
            beginRobotPertThread();
        }
    }
    while (robotPerturbationLive == TRUE)
    {
    }
    printf("\nExperiment finished...\n");
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
    adeptRobot.movetrans(defaultPoint);
    for (int i = 0; i < this->numberOfPerturbations; i++)
    {

        printf("perturbation %d / %d\n", i+1, numberOfPerturbations);
        //adeptRobot.movetrans(defaultPoint);
        angle[0] = defaultPoint.x;
        angle[1] = defaultPoint.y;
        angle[2] = defaultPoint.z;
        angle[3] = defaultPoint.a;
        angle[4] = defaultPoint.b;
        angle[5] = defaultPoint.c;
        if (isFlex==true)
        {
            angle[5] = defaultPoint.c + flexAngle;
            isFlex = false;
            //motorObj->perturbationAngle = flexAngle;
            motorObj->setPerturbationAngle(flexAngle);
        }
        else if (isFlex==false)
        {
            angle[5] = defaultPoint.c + extendAngle;
            isFlex = true;
            //motorObj->perturbationAngle = extendAngle;
            motorObj->setPerturbationAngle(extendAngle);
        }
        motorObj->trialTrigger = 2;
        newPoint = PPoint(angle[0],angle[1],angle[2],angle[3],angle[4],angle[5]);
        motorObj->trialTrigger = 1;
        adeptRobot.movetrans(newPoint);
        Sleep(2000);
    }
    adeptRobot.movetrans(defaultPoint);
    Sleep(2000);
    robotPerturbationLive = FALSE;
    return 1;
}

// SHELL ------------------------------------------------------------------------------------------------------------
void expParadigmCDMRPimplant::sweepShell3D() {
    long i = 0;
    this->numberOfPerturbations = numberOfPerturbations;

    for (; i < numTrials; i++)
    {
        while (robotPerturbationLive == TRUE) {}
        printf("\n\nTrial %ld <%ld Perts, Disp(%3.2lf, %3.2lf, %3.2lf), %2.2lfN Force>\n",
                            i+1, numPerts,   dispX[i], dispY[i], dispZ[i], flexorForce[i]);
        setPerturbationShell(i,numPerts, dispX[i], dispY[i], dispZ[i], flexorForce[i]);
        //motorObj->motorRef[0] = force;
        beginRobotShellThread();
    }

    while (robotPerturbationLive == TRUE) {}
    printf("\nCDMRP Force Shell Experiment finished...\n");
    return;
}

int expParadigmCDMRPimplant::setPerturbationShell(long trial, long perts, double x, double y, double z, double flexF)
{
    this->thisTrial = trial;
    this->Perts     = perts;
    this->X         = x;
    this->Y         = y;
    this->Z         = z;
    this->flexForce = flexF;
    return 1;
}

int expParadigmCDMRPimplant::beginRobotShellThread(void)
{
    robotPerturbationLive = TRUE;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(expParadigmCDMRPimplant::perturbShellAdeptLoop,0,this);
    return 1;
}

void expParadigmCDMRPimplant::perturbShellAdeptLoop(void* a)
{
	((expParadigmCDMRPimplant*)a)->perturbShellAdept();
}

int expParadigmCDMRPimplant::perturbShellAdept()
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

    // Go to default point
    adeptRobot.movetrans(defaultPoint);

    // Set new position
    dispPoint   = defaultPoint;
    dispPoint.x = X;
    dispPoint.y = Y;
    dispPoint.z = Z;

    for (int i = 0; i < Perts; i++)
    {
        printf("\n\nTrial %ld, Pert %ld/%ld, Disp(%3.2lf, %3.2lf, %3.2lf), %2.2lfN Force>\n",
                    thisTrial,     i+1, Perts,         X,      Y,      Z ,     flexForce);
        
        // Move back to default position
        adeptRobot.movetrans(defaultPoint);
        Sleep(shellSleep);
        
        // Close hand - activate flexors (no need to sleep)

        // Move to new position
        adeptRobot.movetrans(dispPoint);
        Sleep(shellSleep);

        // Move back to default position
        adeptRobot.movetrans(defaultPoint);
        Sleep(shellSleep);

        // Open hand - activate extensors (no need to sleep)

        
        
        /*
        //adeptRobot.movetrans(defaultPoint);
        angle[0] = defaultPoint.x;
        angle[1] = defaultPoint.y;
        angle[2] = defaultPoint.z;
        angle[3] = defaultPoint.a;
        angle[4] = defaultPoint.b;
        angle[5] = defaultPoint.c;
        if (isFlex==true)
        {
            angle[5] = defaultPoint.c + flexAngle;
            isFlex = false;
            //motorObj->perturbationAngle = flexAngle;
            motorObj->setPerturbationAngle(flexAngle);
        }
        else if (isFlex==false)
        {
            angle[5] = defaultPoint.c + extendAngle;
            isFlex = true;
            //motorObj->perturbationAngle = extendAngle;
            motorObj->setPerturbationAngle(extendAngle);
        }
        */

        motorObj->trialTrigger = 2;
        
        /*
        newPoint = PPoint(angle[0],angle[1],angle[2],angle[3],angle[4],angle[5]);
        motorObj->trialTrigger = 1;
        adeptRobot.movetrans(newPoint);
        */

        //
        Sleep(2000);
    }
    adeptRobot.movetrans(defaultPoint);
    Sleep(2000);
    robotPerturbationLive = FALSE;
    return 1;
}
//-------------------------------------------------------------------------------------------------------------------

void expParadigmCDMRPimplant::readData()
{
    //numTrials = Trials;    
    //C:\Users\PXI_BBDL\Documents\GitHub\R01v2
    char *configFileName = "CDMRPtest.txt";
    FILE *configFile;
    char header[200];
    configFile = fopen("C:\\Users\\PXI_BBDL\\Documents\\GitHub\\R01v2\\CDMRPtest.txt","r");
    if (configFile == NULL) {
        printf("\n\nCould not open data file.\n\n");
        return;
    }

    std::cout<<"\n\nCDMRP Paradigm: Reading File:";
    // extract header
    fscanf(configFile,"%s\n",&header);
    std::cout << "\n\tHeader: " << header << std::endl;
    
    // extract number of trials
    numTrials = 0;
    fscanf(configFile,"%ld\n",&numTrials);
    if(numTrials > buffSize) {
        numTrials = buffSize;
    }
    std::cout<<"\tNumber of Trials: "<<numTrials<<"\n\n";

    // extract number of perturbations per trial
    numPerts = 0;
    fscanf(configFile,"%ld\n",&numPerts);
    std::cout<<"\tNumber of Perturbations: "<<numPerts<<"\n\n";    
    
    // extract trials
    double tempX = 0, tempY = 0, tempZ = 0, tempF = 0;
    flexorForce = 0;
    for(long i = 0; i < numTrials; i++){
        fscanf(configFile, "%lf,%lf,%lf,%lf\n", &(flexorForce), &(dispX[i]), &(dispY[i]), &(dispZ[i]) );
        //printf("\n\tDisp(X,Y,Z | %ld) = (%lf, %lf, %lf).\n",i, dispX[i], dispY[i], dispZ[i]);
    }

    fclose(configFile);
}

int expParadigmCDMRPimplant::setAdeptDefaultPosition()
{
    defaultPoint = PPoint(defaultPos[0],
                          defaultPos[1],
                          defaultPos[2],
                          defaultPos[3],
                          defaultPos[4],
                          defaultPos[5] );
    printf("\n\nAdept Arm: Default Position Set to:\n\tJ1 = %3.2f\n\tJ2 = %3.2f\n\tJ3 = %3.2f\n\tJ4 = %3.2f\n\tJ5 = %3.2f\n\tJ6 = %3.2f\n",
            defaultPoint.x,
            defaultPoint.y,
            defaultPoint.z,
            defaultPoint.a,
            defaultPoint.b,
            defaultPoint.c );
    return 1;
}