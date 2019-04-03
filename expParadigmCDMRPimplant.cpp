#include "expParadigmCDMRPimplant.h"
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <math.h>

#define shellForce  12  // 12 or 15 or 18 or 21
#define shellSleep  1000
#define ballSleep   1000
#define scaleDisp   5 //in file - if units are mm, set 1
                      //        - if units are cm, set 10

#define ballMinForce        6.0
#define ballMaxForce        21.0
#define ballForceRes        3.0
#define nBallPerts          5
#define ballDisp            50
#define interpolateBallPts  10
#define InterpoBallSlpTime  5000
#define ballGoSlowFlag      0
#define extForce            9

const int Trials = 32; //replace numTrials with Trials and vice versa


expParadigmCDMRPimplant::expParadigmCDMRPimplant(motorControl *temp)
{
    this->motorObj = temp;
     defaultEP[0] =   80.724; // x
     defaultEP[1] = -479.640; // y
     defaultEP[2] =  269.327; // z
     defaultEP[3] = -  0.946; // a
     defaultEP[4] =   88.147; // b
     defaultEP[5] =  136.135; // c
     
     defaultPos[0] = - 91.104; // J1
     defaultPos[1] = - 76.256; // J2
     defaultPos[2] =  176.273; // J3
     defaultPos[3] =   91.798; // J4
     defaultPos[4] =   90.478; // J5
     defaultPos[5] =   36.128; // J6
     
     CDMRPprotocol = 0.0;
     
    //double defaultPos[6] = {154.548,-62.378,158.209,-92.896,-64.234,13.145};

    setAdeptDefaultPosition();
    robotPerturbationLive = false;
    numberOfPerturbations = 0;
    adeptRobot.connectToController();
    numTrials = numPerts = 0;
    readData();
    perturbationAngle = 0;
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

int expParadigmCDMRPimplant::startAdeptPerturbations(int numberOfPerturbations)
{
    this->numberOfPerturbations = numberOfPerturbations;
    robotPerturbationLive = TRUE;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(expParadigmCDMRPimplant::AdeptPerturbationsLoop,0,this);
    return 1;
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

int expParadigmCDMRPimplant::setPerturbationAngle(double angle)
{
    this->perturbationAngle = angle;
    return 1;
}

int expParadigmCDMRPimplant::beginRobotPertThread(void)
{
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
    for (int i = 0; i < this->numberOfPerturbations; i++)
    {

        printf("perturbation %d / %d\n", i+1, numberOfPerturbations);
        //adeptRobot.move(defaultPoint);
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
        //motorObj->trialTrigger = 2;
        newPoint = PPoint(angle[0],angle[1],angle[2],angle[3],angle[4],angle[5]);
        //motorObj->trialTrigger = 1;
        adeptRobot.move(newPoint);
        Sleep(2000);
    }
    adeptRobot.move(defaultPoint);
    Sleep(2000);
    robotPerturbationLive = FALSE;
    return 1;
}

// SHELL ------------------------------------------------------------------------------------------------------------
void expParadigmCDMRPimplant::sweepShell3D() {
    long i = 0, lineNum = 1;
    this->numberOfPerturbations = numberOfPerturbations;

    int tempTrial = motorObj->CDMRPtrialFlag;

    CDMRPprotocol = 1.0;

    //Select line/trial number with which to begin
    printf("\n\nSelect starting line number [1 ~ %ld]: \n\n", numTrials);
    do{
            scanf("%ld", &lineNum);
            if ( (lineNum > numTrials) || (lineNum < 1) )
                printf("Wrong input! Permitted Line numbers are 1 ~ %ld. Try again: \n", numTrials);
    } while    ( (lineNum > numTrials) || (lineNum < 1) );

    for (i = lineNum-1; i < numTrials; i++)
    {
        while (robotPerturbationLive == TRUE) {}
        printf("\n\n\n3D Shell Trial %3ld/%3ld: Disp(%5.2lf, %5.2lf, %5.2lf), %5.2lfN Force\n",
                            i+1, numTrials,   dispX[i], dispY[i], dispZ[i], flexorForce[i]);
        printf("---------------------------------------------------------------\n");
        
        //setPerturbationShell(i,numPerts, dispX[i], dispY[i], dispZ[i], flexorForce[i]);
        setPerturbationShell(i,numPerts, dispX[i], dispY[i], dispZ[i], (double) shellForce); // setting flexor force

        motorObj->CDMRPtrialFlag = (int) (i+1);
        beginRobotShellThread();
    }

    while (robotPerturbationLive == TRUE) {}
    motorObj->CDMRPtrialFlag = tempTrial;
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

    //double extForce = flexForce;

    bool isFlex = true;
    //adeptRobot.setVelocity (10, 10, MONITOR, true);

    // Go to default point
    adeptRobot.move(defaultPoint);

    // Set new position
    dispPoint   = defaultPoint;
    dispPoint.x = X;
    dispPoint.y = Y;
    dispPoint.z = Z;

    backPoint   = defaultPoint;
    backPoint.x = -X;
    backPoint.y = -Y;
    backPoint.z = -Z;
    
    // open hand
        // activate extensors
    motorObj->motorRef[2] = extForce; // finger 2 extensor
    motorObj->motorRef[3] = extForce; // thumb  extensor
    motorObj->motorRef[4] = extForce; // finger 3 extensor
    motorObj->motorRef[5] = extForce; // finger 4 extensor
    motorObj->motorRef[6] = extForce; // finger 5 extensor

        // tone out flexors
    motorObj->motorRef[0] = toneForce; // finger flexors
    motorObj->motorRef[1] = toneForce; // thumb  flexors

    // Move back to default position
    adeptRobot.move(defaultPoint);
    Sleep(shellSleep/2);

    // close hand
        // activate flexors
    motorObj->motorRef[0] = flexForce; // finger flexors
    motorObj->motorRef[1] = flexForce; // thumb  flexors
        // tone out extensors
    motorObj->motorRef[2] = toneForce; // finger 2 extensor
    motorObj->motorRef[3] = toneForce; // thumb  extensor
    motorObj->motorRef[4] = toneForce; // finger 3extensor
    motorObj->motorRef[5] = toneForce; // finger 4 extensor
    motorObj->motorRef[6] = toneForce; // finger 5 extensor
        
    //exp paradigm trial tick

    int tempPert    = motorObj->CDMRPpertuFlag;
    int tempWatch   = motorObj->CDMRPwatchFlag;

    for (long i = 0; i < Perts; i++)
    {
        printf("\n\n\n\t3D Shell Pert %ld/%ld: Disp(%3.2lf, %3.2lf, %3.2lf), %2.2lfN Force (Trial %ld)\n",
                                   i+1, Perts,           X,      Y,      Z ,     flexForce, thisTrial);
        // perturb tick
        motorObj->CDMRPpertuFlag = (int) (i+1);

        // Move to new position
        printf("\n\n\t\t\tGo to shell\n");
        adeptRobot.movetrans(dispPoint);
        motorObj->CDMRPwatchFlag = 1;
        Sleep(shellSleep);
        motorObj->CDMRPwatchFlag = tempWatch;

        // MOVE BACK to default position
        printf("\n\n\t\t\tReturn to default\n");
        adeptRobot.movetrans(backPoint);
        Sleep(shellSleep/2);

        //motorObj->trialTrigger = 2;        
    }

    motorObj->motorRef[0] = toneForce;
    motorObj->motorRef[1] = toneForce;
    motorObj->motorRef[2] = toneForce;
    motorObj->motorRef[3] = toneForce;
    motorObj->motorRef[4] = toneForce;
    motorObj->motorRef[5] = toneForce;
    motorObj->motorRef[6] = toneForce;

    adeptRobot.move(defaultPoint);
    Sleep(shellSleep/2);
    
    motorObj->CDMRPpertuFlag = tempPert;
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
    
    for(long i = 0; i < numTrials; i++){
        flexorForce[i] = 0;
        dispX[i] = 0; dispY[i] = 0; dispZ[i] = 0;
        fscanf(configFile, "%lf,%lf,%lf\n", &(dispX[i]), &(dispY[i]), &(dispZ[i]));
        //fscanf(configFile, "%lf,%lf,%lf,%lf\n", &(dispX[i]), &(dispY[i]), &(dispZ[i]), &(flexorForce[i]) );
        //printf("\n\tDisp(X,Y,Z | %ld) = (%lf, %lf, %lf) : Tendon Force %2.2lfN.\n",i+1, dispX[i], dispY[i], dispZ[i], flexorForce[i]);
        
        // Scale displacements
        dispX[i] *= scaleDisp;
        dispY[i] *= scaleDisp;
        dispZ[i] *= scaleDisp;
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
    printf("\n\nAdept Arm: Default Position Set to:\n\tJ1 = %3.2f\n\tJ2 = %3.2f\n\tJ3 = %3.2f\n\tJ4 = %3.2f\n\tJ5 = %3.2f\n\tJ6 = %3.2f\n\n",
            defaultPoint.x,
            defaultPoint.y,
            defaultPoint.z,
            defaultPoint.a,
            defaultPoint.b,
            defaultPoint.c );
    return 1;
}

// BALL PULL -----------------------------------------------------------------------------------------------------------
void expParadigmCDMRPimplant::sweepBallPull(double minForce, double maxForce, double forceRes, unsigned int numPerturbs)
{
    printf("\n\nCDMRP Ball Pulling paradigm started...\n\n");    
    unsigned int NumBallTrials = (unsigned int)((maxForce-minForce)/forceRes + 1);
    unsigned int    trialNum = 1;
    
    printf("\n\n\n\t\t%d Trials (%d reps): Min Force: %6.2ld, Force Steps: %6.2ld, Min Force: %6.2ld\n\n\n",
                  NumBallTrials, numPerturbs, minForce, forceRes, maxForce);

    int tempTrial = motorObj->CDMRPtrialFlag;
        
    for(double f = minForce; f <= maxForce; f += forceRes, trialNum++) {
        while (robotPerturbationLive == TRUE) {}
        printf("\n\n\nBall Pull %02u/%02u: %05.2fN target flexor tension, %02u perturbations.\n", trialNum, NumBallTrials, f, numPerturbs);
        printf("----------------------------------------------------------------\n");
        //oneBallPull(/*f, trialNum, numPerturbs*/);
        motorObj->CDMRPtrialFlag = trialNum;
        beginRobotBallThread(f, trialNum, numPerturbs);
        //Sleep(ballSleep);
    }

    while (robotPerturbationLive == TRUE) {}
    motorObj->CDMRPtrialFlag = tempTrial;
    printf("\n\nCDMRP Ball Pulling paradigm finished!!\n\n");
}

int expParadigmCDMRPimplant::beginRobotBallThread(double f, unsigned int trialNum, unsigned int numPerturbs)
{
    robotPerturbationLive = TRUE;
    ball_f          = f;
    ball_trial      = trialNum;
    ball_pertCount  = numPerturbs;
    hIOMutex = CreateMutex(NULL, FALSE, NULL);
	_beginthread(expParadigmCDMRPimplant::perturbBallLoop,0,this);
    return 1;
}

void expParadigmCDMRPimplant::perturbBallLoop(void* a)
{
	((expParadigmCDMRPimplant*)a)->oneBallPull();
}

void expParadigmCDMRPimplant::oneBallPull()
{
    double          flexorTension   = ball_f;
    unsigned int    trialNum        = ball_trial;
    unsigned int    numPerturbations= ball_pertCount;
    
    // Setup disp position
    dispPoint   = defaultPoint;
    dispPoint.x = -ballDisp;
    dispPoint.y = 0;
    dispPoint.z = 0;

    // setup back position
    backPoint   = defaultPoint;
    backPoint.x = ballDisp;
    backPoint.y = 0;
    backPoint.z = 0;

    // tone out all muscles
    motorObj->motorRef[0] = toneForce; // finger flexors
    motorObj->motorRef[1] = toneForce; // thumb  flexors
    motorObj->motorRef[2] = toneForce; // finger extensors
    motorObj->motorRef[3] = toneForce; // thumb  extensors
    motorObj->motorRef[4] = toneForce; // finger extensors
    motorObj->motorRef[5] = toneForce; // finger extensors
    motorObj->motorRef[6] = toneForce; // finger extensors
    
    flexForce       = flexorTension;

    int tempPert    = motorObj->CDMRPpertuFlag;
    int tempWatch   = motorObj->CDMRPwatchFlag;
    //double extForce = flexForce;
    for (unsigned int pert = 0; pert < numPerturbations; pert++) {
        printf("\n\n\n\tBall Pull Pert %u/%u: %5.2fN target flexor tension (Trial %u).\n", pert+1, numPerturbations, flexorTension, trialNum);
                
        // pert tick
        motorObj->CDMRPpertuFlag = (int) (pert+1);
    
        // open hand
        printf("\n\n\t\t\tOpening hand.\n");
            // activate extensors
        motorObj->motorRef[2] = extForce; // finger 2 extensor
        motorObj->motorRef[3] = extForce; // thumb  extensor
        motorObj->motorRef[4] = extForce; // finger 3extensor
        motorObj->motorRef[5] = extForce; // finger 4 extensor
        motorObj->motorRef[6] = extForce; // finger 5 extensor

            // tone out flexors
        motorObj->motorRef[0] = toneForce; // finger flexors
        motorObj->motorRef[1] = toneForce; // thumb  flexors
        Sleep(ballSleep*3);

        // perturbation tick

        // put ball in position and set movement velocity, then sleep
        printf("\n\n\t\t\tMoving adept to default.\n");
        adeptRobot.move(defaultPoint);
        Sleep(ballSleep/2);
        //printf("\n\n\t\t\tSetting adept speed.\n");
        //adeptRobot.setVelocity(2, 2, VelocityType::MILLIMETERS_PER_SECOND, 0);
        //Sleep(ballSleep);

        // close hand
            // tone out extensors
        motorObj->motorRef[2] = toneForce; // finger 2 extensor
        motorObj->motorRef[3] = toneForce; // thumb  extensor
        motorObj->motorRef[4] = toneForce; // finger 3extensor
        motorObj->motorRef[5] = toneForce; // finger 4 extensor
        motorObj->motorRef[6] = toneForce; // finger 5 extensor
        Sleep(ballSleep);
            // activate flexors
        printf("\n\n\t\t\tClosing hand.\n");
        motorObj->motorRef[0] = flexForce; // finger flexors
        motorObj->motorRef[1] = flexForce; // thumb  flexors
        Sleep(ballSleep*3);
        
        // pull hand out
        printf("\n\n\t\t\tPulling out hand.\n");
        motorObj->CDMRPwatchFlag = 1;
        if (ballGoSlowFlag == 1) {
            dispPoint.x = dispPoint.x / interpolateBallPts;
            for(unsigned int j = 0; j < interpolateBallPts; j++) {
                adeptRobot.movetrans(dispPoint);
                Sleep(InterpoBallSlpTime/interpolateBallPts);
            }
        } else {
            adeptRobot.movetrans(dispPoint);
            Sleep(ballSleep*0.75);
        }
        motorObj->CDMRPwatchFlag = tempWatch;

        // tone out all muscles
    motorObj->motorRef[0] = toneForce; // finger flexors
    motorObj->motorRef[1] = toneForce; // thumb  flexors
    motorObj->motorRef[2] = toneForce; // finger extensors
    motorObj->motorRef[3] = toneForce; // thumb  extensors
    motorObj->motorRef[4] = toneForce; // finger extensors
    motorObj->motorRef[5] = toneForce; // finger extensors
    motorObj->motorRef[6] = toneForce; // finger extensors
    Sleep(ballSleep*3);

    }

    // open hand
        // activate extensors
    motorObj->motorRef[2] = extForce; // finger 2 extensor
    motorObj->motorRef[3] = extForce; // thumb  extensor
    motorObj->motorRef[4] = extForce; // finger 3extensor
    motorObj->motorRef[5] = extForce; // finger 4 extensor
    motorObj->motorRef[6] = extForce; // finger 5 extensor        
        // tone our flexors
    motorObj->motorRef[0] = toneForce; // finger flexors
    motorObj->motorRef[1] = toneForce; // thumb  flexors
    Sleep(ballSleep);

    // Go to default point
    adeptRobot.move(defaultPoint);
    Sleep(ballSleep/2);

    // tone out all muscles
    motorObj->motorRef[0] = toneForce; // finger flexors
    motorObj->motorRef[1] = toneForce; // thumb  flexors
    motorObj->motorRef[2] = toneForce; // finger extensors
    motorObj->motorRef[3] = toneForce; // thumb  extensors
    motorObj->motorRef[4] = toneForce; // finger extensors
    motorObj->motorRef[5] = toneForce; // finger extensors
    motorObj->motorRef[6] = toneForce; // finger extensors

    std::cout<<std::endl<<std::endl;
    motorObj->CDMRPpertuFlag = tempPert;
    robotPerturbationLive = FALSE;    
}
//----------------------------------------------------------------------------------------------------------------------

// SUB MENU ------------------------------------------------------------------------------------------------------------
void expParadigmCDMRPimplant::CDMRPmenu()
{
    int menu = 0;
    int tempProtocol;

    // Put motor force activations here
    motorObj->motorRef[0] = toneForce; // finger flexors
    motorObj->motorRef[1] = toneForce; // thumb  flexors
    motorObj->motorRef[2] = toneForce; // finger extensors
    motorObj->motorRef[3] = toneForce; // thumb  extensors
    motorObj->motorRef[4] = toneForce;
    motorObj->motorRef[5] = toneForce;
    motorObj->motorRef[6] = toneForce;

    printf("\n\nCMDRP submenu:\n\t0. Exit.\n\t1. 3D shell movement.\n\t2. Ball pulling.\n\t Select option...\n\n");
    do{
            scanf("%d", &menu);
            if ( (menu > 2) || (menu < 0) )
                printf("Wrong input! try Again.\n");
    } while    ( (menu > 2) || (menu < 0) );

    tempProtocol = motorObj->CDMRPprotoFlag;
    motorObj->CDMRPprotoFlag = (int) menu;

    switch(menu) {
    case 1:
        sweepShell3D();        
        break;
    case 2:
        sweepBallPull(ballMinForce, ballMaxForce, ballForceRes, nBallPerts);
        break;
    default:
        break;
    }// end menu switch

    motorObj->CDMRPprotoFlag = tempProtocol;

    motorObj->motorRef[0] = toneForce;
    motorObj->motorRef[1] = toneForce;
    motorObj->motorRef[2] = toneForce;
    motorObj->motorRef[3] = toneForce;
    motorObj->motorRef[4] = toneForce;
    motorObj->motorRef[5] = toneForce;
    motorObj->motorRef[6] = toneForce;
}
//-------------------------------------------------------------------------------------------------------------------