#include "expParadigmMuscleLengthCalibration.h"
#include "motorControl.h"
#include <servoControl.h>
#include <utilities.h>
#include <NIDAQmx.h>
#include <utilities.cpp>
float ggain,tbias;
expParadigmMuscleLengthCalibration::expParadigmMuscleLengthCalibration()
{
    printf("Enter initial position (degrees):\t");
    scanf("%d",initPos);
    printf("\n Enter final position (degrees):\t");
    scanf("%d",finalPos);
    printf("\n");
}
int expParadigmMuscleLengthCalibration::startParadigm(servoControl *servo, motorControl *realTimeController)
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
  realTimeController->resetMuscleLength = 1;
  Sleep(5);
  //Read Encoder data at initPos
  muscleLengthBeforePert[0] = realTimeController->muscleLength[0];
  muscleLengthBeforePert[1] = realTimeController->muscleLength[1];    
  servo->setPosition(finalPos);
  //Read Encoder data at finalPos
  Sleep(holdPeriod);
  muscleLengthAfterPert[0] = realTimeController->muscleLength[0];
  muscleLengthAfterPert[1] = realTimeController->muscleLength[1];
  servo->goDefault();
  gain[0] = 0.8/(muscleLengthAfterPert[0] - muscleLengthBeforePert[0]);
  gain[1] = 0.8/(muscleLengthBeforePert[1] - muscleLengthAfterPert[1]);
  bias[0] = 1.4 - (gain[0] * muscleLengthAfterPert[0]);
  bias[1] = 1.4 - (gain[1] * muscleLengthBeforePert[1]);
  realTimeController->encoderBias[0] = bias[0];
  realTimeController->encoderBias[1] = bias[1];
  realTimeController->encoderGain[0] = gain[0];
  realTimeController->encoderGain[1] = gain[1];
  GGAIN = ggain;
  TBIAS = tbias;
}
expParadigmMuscleLengthCalibration::~expParadigmMuscleLengthCalibration()
{

}