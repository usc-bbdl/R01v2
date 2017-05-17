#include "expParadigmMuscleLengthCalibration.h"
#include "motorControl.h"
#include "servoControl.h"
#include "utilities.h"

expParadigmMuscleLengthCalibration::expParadigmMuscleLengthCalibration(servoControl *param)
{
    servo = param;
    rampVelocity = 100;
    holdPeriod = 2000;
    initPos = -90;
    finalPos = 90;
}

expParadigmMuscleLengthCalibration::~expParadigmMuscleLengthCalibration()
{

}
int expParadigmMuscleLengthCalibration::setInitFinalPos(int initPos, int finalPos)
{
    this->initPos = initPos;
    this->finalPos = finalPos;
    return 1;
}
int expParadigmMuscleLengthCalibration::startParadigm(motorControl *realTimeController)
{
  realTimeController->resetLength();
  Sleep(20);
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
      if (i % 2 == 0)
      {
          encoderGain[i] = 1/(muscleLengthBeforePert[i] - muscleLengthAfterPert[i]);
          encoderBias[i] = 0.6 - (encoderGain[i] * muscleLengthAfterPert[i]);
      }
      else
      {
          encoderGain[i] = 1/(muscleLengthAfterPert[i] - muscleLengthBeforePert[i]);
          encoderBias[i] = 0.6 - (encoderGain[i] * muscleLengthBeforePert[i]);
          
      }
  }
  
  realTimeController->setEncoderCalibration(encoderGain,encoderBias);
  realTimeController->setForceGainOffset(fpgaForceGain,fpgaForceBias);
  printf("Scaling from Encoder to muscle length is updated: Calibration finished. \n");
  printf("Press space to continue.\n");
  return 1;
}
