#pragma once
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <process.h>
#include <math.h>
#include "include/dynamixel.h"
#include "TimeData.h"

#ifdef _WIN64
#pragma comment(lib, "lib/x64/dynamixel.lib")
#else
#pragma comment(lib, "lib/x86/dynamixel.lib")
#endif

class KinematicPerturbation
{
	static const int DEFAULT_PORTNUM = 4;		//COM4
	static const int DEFAULT_BAUDNUM = 34;		//157200 bps
	static const int DEFAULT_ID= 		1;

	static const int P_GOAL_POSITION_L		= 30;
	static const int P_GOAL_POSITION_H	= 	31;
	static const int P_MOVING_SPEED_L	= 	32;
	static const int P_MOVING_SPEED_H	= 	33;
	static const int P_PRESENT_POSITION_L= 	36;
	static const int P_PRESENT_POSITION_H= 	37;

	HANDLE hIOMutex;

	int goalPos[2];
	int movingSpeed[2];
	int goalPosIndex;
	int movingSpeedIndex;
	int currentPos;
	int delayThread;
	int sineWavePos;
	int sineWave[2000];
	int motorPositionBits[3500];

	bool isInitialized;
	bool isPerturbing;
	bool isChangingSpeed;
	bool isSineWaveLookUp;
	bool isSineWaveTimer;
	bool isRamping;
	bool isPerturbationFromFile;
	bool kill;

	double rampUpTime;

	char sFileName[400];
	FILE *perturbationFile;
	FILE *dataFile;
	bool bReceivedTimeStamp;
	int eventCounter;

public:
	TimeData timeData;
	bool isRecording;
	KinematicPerturbation(void);
	~KinematicPerturbation(void);
	static void StaticRecordingLoop(void*);
	void RecordingLoop(void);
	void Perturb(void);
	void RampHoldandBack(void);
	void SineWavePerturbationLookUpTable(void);
	int SineWavePerturbationTimer(void);
	void StopPerturbing(void);
	void ReadServoPosition(void);
	void SendServoPosition(int);
	void ChangeServoSpeed(void);
	void ChangeSpeed(int);
	int startRecording();
	int stopRecording();
	int setTimeStamp(char *);
	void TransformInitialandFinalPosition(float,float);

	void ReadPerturbationFile(void);
	void TrackPerturbationFromFile(void);
};

