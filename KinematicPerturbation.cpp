#include "KinematicPerturbation.h"

//#define FAST_PERTURBATION


KinematicPerturbation::KinematicPerturbation(void)
{

	   /*goalPos[0] = (int)((150 + neutralPos)*1024/300); 
	   goalPos[1] = (int)((212 + neutralPos)*1024/300);*/
	//TransformInitialandFinalPosition(10.0,-62.0);	


	//goalPos[0] = 368;
	//goalPos[1] = 614;
	goalPos[0] = 614;
	goalPos[1] = 368;
	bReceivedTimeStamp = false;
	eventCounter = 0;
	isRecording = false;

	goalPosIndex = 0;
	
	movingSpeed[0] = 100;
	movingSpeed[1] = 1023;
	isPerturbing = false;
	isChangingSpeed = false;
	isRamping = false;

	isInitialized = dxl_initialize(DEFAULT_PORTNUM,DEFAULT_BAUDNUM);
	if(isInitialized) {
		hIOMutex = CreateMutex(NULL, FALSE, NULL);
		kill = 0;
		delayThread = 1;
		_beginthread(KinematicPerturbation::StaticRecordingLoop,0,this);
	}	
}


KinematicPerturbation::~KinematicPerturbation(void)
{
	if(isInitialized) {
		dxl_terminate();
		kill = 1;
		fclose(dataFile);
	}
}

void KinematicPerturbation::TransformInitialandFinalPosition(float initialPos, float excursion)
{
	float finalPos;
	if(initialPos > 10) {
	    initialPos = 10;
	}
	else if(initialPos < -90) {
	    initialPos = -90;
	}
	
	finalPos = initialPos + excursion;
	if(finalPos < -90) {
	    finalPos = -90;
	}
	else if(finalPos > 10) {
	    finalPos = 10;
	}
	else {
		
	}
	goalPos[0] = (int)((150 + initialPos)*1024/300);
	goalPos[1] = (int)((150 + finalPos)*1024/300);
}

void KinematicPerturbation::ReadPerturbationFile(void)
 {
	double motorPosition[3500];

	perturbationFile = fopen("perturbation_profile.txt","r");
	if (perturbationFile != NULL) {
		for(int i = 0; i < 3000; i++)
		{
		    fscanf(perturbationFile, "%lf\n", &motorPosition[i]);
			motorPositionBits[i] = (int)((512 + 1024.0/300.0*6*motorPosition[i]));
		}
	}		
 }

void KinematicPerturbation::TrackPerturbationFromFile(void)
{
	isPerturbationFromFile = true;
	
}

void KinematicPerturbation::RampHoldandBack(void)
{
	if(!isRamping) {
	    isRamping = true;
#ifdef FAST_PERTURBATION
		rampUpTime = timeData.getCurrentTime() + 2;
#else
		rampUpTime = timeData.getCurrentTime() + 3.0;
#endif
	    Perturb();
	}
}

void KinematicPerturbation::Perturb(void)
{
	isPerturbing = true;
	if(goalPosIndex == 0) {
		goalPosIndex = 1;
	}
	else {
		goalPosIndex = 0;
	}
}

void KinematicPerturbation::SineWavePerturbationLookUpTable(void)
{
	int amplitude = (goalPos[1] - goalPos[0])/2;
	int offset = 512;

	for(int i = 0; i < 25; i++) {
		sineWave[i] =  (int)(amplitude*sin(6.2839*i/25.0) + offset);
	}
	isSineWaveLookUp = true;
}

int KinematicPerturbation::SineWavePerturbationTimer(void)
{
	isSineWaveTimer = true;

	int amplitude = (goalPos[1] - goalPos[0])/2;
	int offset = 512;
	int sineWavePos;

	sineWavePos =  (int)(amplitude*sin(6.2839*2*timeData.getCurrentTime()) + offset);
	return sineWavePos;
}

void KinematicPerturbation::SendServoPosition(int position)
{
	dxl_write_word( DEFAULT_ID, P_GOAL_POSITION_L, position);
}

void KinematicPerturbation::ReadServoPosition(void)
{
	currentPos = dxl_read_word( DEFAULT_ID, P_PRESENT_POSITION_L );
}

void KinematicPerturbation::StopPerturbing(void)
{
	isSineWaveLookUp = false;
	isSineWaveTimer = false;
	isPerturbing = false;
}

void KinematicPerturbation::ChangeServoSpeed(void)
{
	isChangingSpeed = true;
	if(movingSpeedIndex == 0) {
		movingSpeedIndex = 1;
	}
	else {
		movingSpeedIndex = 0;
	}
}

void KinematicPerturbation::ChangeSpeed(int speed)
{
    dxl_write_word( DEFAULT_ID, P_MOVING_SPEED_L, speed);
}

void KinematicPerturbation::StaticRecordingLoop(void* a)
{
	((KinematicPerturbation*)a)->RecordingLoop();
}

void KinematicPerturbation::RecordingLoop(void)
{
	/*double tim = 0.0;
	time_t rawTime;
	struct tm *timeInfo;
	time(&rawtime);
	timeInfo = localtime(&rawTime);*/	
	int i = 0;
	int j = 0;

	//kinematicFile = fopen(sFileName, "w");
#ifdef FAST_PERTURBATION
	dxl_write_word( DEFAULT_ID, P_MOVING_SPEED_L, 1023);
#else
	dxl_write_word( DEFAULT_ID, P_MOVING_SPEED_L, 50);
#endif
	while(!kill) {	
		WaitForSingleObject(hIOMutex, INFINITE);
		if(isChangingSpeed) {
			dxl_write_word( DEFAULT_ID, P_MOVING_SPEED_L, movingSpeed[movingSpeedIndex]);
			isChangingSpeed = false;
		}
		if(isSineWaveLookUp) {
			if(i > 25) {
				i = 0;
			}
			dxl_write_word( DEFAULT_ID, P_GOAL_POSITION_L, sineWave[i]);
			i++;		
		}
		if(isSineWaveTimer)
		{
			SendServoPosition(SineWavePerturbationTimer());
		}
		if(isPerturbing) {
			dxl_write_word( DEFAULT_ID, P_GOAL_POSITION_L, goalPos[goalPosIndex]);
			isPerturbing = false;	
		}

		if(isRamping) {
			if(timeData.getCurrentTime() >= rampUpTime) {
				Perturb();
				isRamping = false;
			}	
		}

		if(isPerturbationFromFile)
		{
			if(j > 3000)
			{
			    j = 0;
			    isPerturbationFromFile = false;
			}
			dxl_write_word( DEFAULT_ID, P_GOAL_POSITION_L, motorPositionBits[j]);
			j+=2;
		}

		if(bReceivedTimeStamp) {
			if (eventCounter == 0) {
				dataFile = fopen(sFileName, "w");
				fprintf(
					dataFile,
					"time,pos\n"
				);
				eventCounter++;
			}
			else if(isRecording) {
				//ReadServoPosition();
				//fprintf(dataFile,"%f,%d\n",timeData.getCurrentTime(),currentPos);
			}
		}
		ReleaseMutex( hIOMutex);
	}
}

int KinematicPerturbation::startRecording()
{
	isRecording = true;
	return 0;
}

int KinematicPerturbation::stopRecording()
{
	isRecording = false;
	bReceivedTimeStamp = false;
	isPerturbing = false;
	isChangingSpeed = false;
	isRamping = false;
	isSineWaveTimer = false;
	eventCounter = 0;
	fclose(dataFile);
	return 0;
}


int KinematicPerturbation::setTimeStamp(char *timeStamp)
{
	sprintf_s(
		sFileName, 
		"C:\\data\\%s\.dev_kinematic\.txt",
		timeStamp
	);

	bReceivedTimeStamp = true;

	return 0;
}