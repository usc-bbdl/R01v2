#include "expParadigmMuscleLengthCalibration.h"
#include "motorControl.h"
#include <servoControl.h>
#include <utilities.h>
#include <NIDAQmx.h>
float ggain,tbias;
expParadigmMuscleLengthCalibration::expParadigmMuscleLengthCalibration(servoControl *param)
{
    servo = param;
    rampVelocity = 10;
    holdPeriod = 10000;
    //printf("Enter initial position (degrees): ");
    //std::cin>>initPos;
    //printf("\n Enter final position (degrees):\t");
    //std::cin>>finalPos;
    //printf("\n");
    initPos = 60;
    finalPos = 0;
}
int expParadigmMuscleLengthCalibration::startParadigm(motorControl *realTimeController)
{
  float64 muscleLengthBeforePert[2], muscleLengthAfterPert[2];
  realTimeController->encoderBias[0] = realTimeController->encoderBias[1] = 0;
  realTimeController->encoderGain[0] = realTimeController->encoderGain[1] = 1;
  ggain = GGAIN;
  tbias = TBIAS;
  GGAIN = 0;
  TBIAS = 1;
  //code here
  servo->setVelocity(rampVelocity);
  servo->setPosition(initPos);
  Sleep(holdPeriod);
  //realTimeController->resetMuscleLength = 1;
  Sleep(10);
  //Read Encoder data at initPos
  muscleLengthBeforePert[0] = realTimeController->muscleLength[0];
  muscleLengthBeforePert[1] = realTimeController->muscleLength[1];    
  servo->setPosition(finalPos);
  //Read Encoder data at finalPos
  Sleep(holdPeriod);
  muscleLengthAfterPert[0] = realTimeController->muscleLength[0];
  muscleLengthAfterPert[1] = realTimeController->muscleLength[1];
  servo->goDefault();
  gain[0] = 0.4/(muscleLengthAfterPert[0] - muscleLengthBeforePert[0]);
  gain[1] = 0.4/(muscleLengthBeforePert[1] - muscleLengthAfterPert[1]);
  bias[0] = 1.2 - (gain[0] * muscleLengthAfterPert[0]);
  bias[1] = 1.2 - (gain[1] * muscleLengthBeforePert[1]);
  realTimeController->encoderBias[0] = bias[0];
  realTimeController->encoderBias[1] = bias[1];
  realTimeController->encoderGain[0] = gain[0];
  realTimeController->encoderGain[1] = gain[1];
  GGAIN = ggain;
  TBIAS = tbias;
  printf("Calibration finished. Press space to continue.\n");
  return 1;
}
expParadigmMuscleLengthCalibration::~expParadigmMuscleLengthCalibration()
{

}