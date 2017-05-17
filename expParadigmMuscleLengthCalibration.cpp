#include "expParadigmMuscleLengthCalibration.h"
#include "motorControl.h"
#include "servoControl.h"
#include "utilities.h"
//#include <NIDAQmx.h>

expParadigmMuscleLengthCalibration::expParadigmMuscleLengthCalibration(servoControl *param)
{
    servo = param;
    rampVelocity = 100;
    holdPeriod = 2000;
    //printf("Enter initial position (degrees): ");
    //std::cin>>initPos;
    //printf("\n Enter final position (degrees):\t");
    //std::cin>>finalPos;
    //printf("\n");
    initPos = -90;
    finalPos = 90;
}

int expParadigmMuscleLengthCalibration::startParadigm(motorControl *realTimeController)
{
  double fpgaForceGain = 0, fpgaForceBias = 0;
  float64 * muscleLengthBeforePert, * muscleLengthAfterPert, * encoderGain, * encoderBias;
  int numberOfMuscles = 0;
  realTimeController->getNumberOfMuscles(&numberOfMuscles);
  muscleLengthBeforePert = new float64 [numberOfMuscles];
  muscleLengthAfterPert = new float64 [numberOfMuscles];
  encoderGain = new float64 [numberOfMuscles];
  encoderBias = new float64 [numberOfMuscles];
  for (int i = 0; i<numberOfMuscles;i++)
  {
      muscleLengthBeforePert[i] = 0;
      muscleLengthAfterPert[i] = 0;
      encoderGain[i] = 1;
      encoderBias[i] = 0;
  }
  realTimeController->getForceGainOffset(&fpgaForceGain, &fpgaForceBias);
  realTimeController->setForceGainOffset(0,2);
  realTimeController->setEncoderCalibration(encoderGain, encoderBias);
  //code here
  servo->setVelocity(rampVelocity);
  servo->setPosition(initPos);
  Sleep(holdPeriod);
  realTimeController->resetLength();
  Sleep(10);
  //Read Encoder data at initPos
  realTimeController->getNumberOfMuscles(&numberOfMuscles);
  realTimeController->getMuscleLength(muscleLengthBeforePert);
  servo->setPosition(finalPos);
  //Read Encoder data at finalPos
  Sleep(holdPeriod);
  realTimeController->getMuscleLength(muscleLengthAfterPert);
  servo->goDefault();
  for (int i = 0; i<numberOfMuscles;i++)
  {
      gain[i] = 1/(muscleLengthBeforePert[i] - muscleLengthAfterPert[i]);
      bias[i] = 0.6 - (gain[i] * muscleLengthAfterPert[i]);
  }
  printf("Muscle Length Before Pert: %f\n",muscleLengthBeforePert[1]);
  printf("Muscle Length After Pert: %f\n",muscleLengthAfterPert[1]);
  printf("Gain is: %f\n",gain[1]);
  printf("Bias is: %f\n",bias[1]);
  realTimeController->setEncoderCalibration(encoderGain,encoderBias);
  realTimeController->setForceGainOffset(fpgaForceGain,fpgaForceBias);
  printf("Calibration finished. Press space to continue.\n");
  return 1;
}
expParadigmMuscleLengthCalibration::~expParadigmMuscleLengthCalibration()
{

}