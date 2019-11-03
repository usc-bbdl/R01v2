#include "expParadigmMuscleLengthCalibration.h"
#include "motorControl.h"
#include <servoControl.h>
#include <utilities.h>
#include <NIDAQmx.h>
float ggain,tbias;
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

    /*
    int flexPos     = -57;
    int extPos      = -7;
    int centerPos   = (flexPos + extPos) / 2;
    */

    initPos  = servo->extPos ;//-18; // extension
    finalPos = servo->flexPos;//-72; // flexion
    printf("\n\nInitial Position (ext) = %d, Final Position (flex) = %d\n\n", initPos, finalPos);

}
int expParadigmMuscleLengthCalibration::startParadigm(motorControl *realTimeController)
{
  float64 muscleLengthBeforePert[2], muscleLengthAfterPert[2];
  realTimeController->encoderBias[0] = realTimeController->encoderBias[1] = 0;
  realTimeController->encoderGain[0] = realTimeController->encoderGain[1] = 1;
  
  //ggain = GGAIN;
  //tbias = TBIAS;
  //GGAIN = 0;
  //TBIAS = 1;


  // MOVE FINGER MANUALLY INSTEAD OF WITH SERVO -------
  float64 newForce = 10; // Newtons
  float64 tempFlex = realTimeController->motorRef[0];
  float64 tempExtend = realTimeController->motorRef[1];

  // EXTEND
  
  servo->setVelocity(rampVelocity);
  servo->setPosition(initPos);
  Sleep(holdPeriod);
  /*
  printf("\n\nExtensor tension: Old = %lf N. New = %lf N.\n", tempExtend, newForce);
  realTimeController->motorRef[1] = newForce;
  Sleep(5000);
  */
  realTimeController->resetMuscleLength = 1;
  Sleep(10);
        //Read Encoder data at initPos
  muscleLengthBeforePert[0] = realTimeController->muscleLength[0];
  muscleLengthBeforePert[1] = realTimeController->muscleLength[1]; 
  //printf("Resetting Extensor.\n\n");
  //realTimeController->motorRef[1] = tempExtend;
  Sleep(1000);
  
  // FLEX
  
  servo->setPosition(finalPos);
  Sleep(holdPeriod);
  /*
  printf("\n\nflexor tension: Old = %lf N. New = %lf N.\n", tempFlex, newForce);
  realTimeController->motorRef[0] = newForce;
  Sleep(5000);
  */
          //Read Encoder data at finalPos
  muscleLengthAfterPert[0] = realTimeController->muscleLength[0];
  muscleLengthAfterPert[1] = realTimeController->muscleLength[1];
  //printf("Resetting Flexor.\n\n");
  //realTimeController->motorRef[0] = tempFlex;
  Sleep(1000);
  
  servo->goDefault();
  

  gain[0] = 1/(muscleLengthBeforePert[0] - muscleLengthAfterPert[0]);
  gain[1] = 1/(muscleLengthAfterPert[1] - muscleLengthBeforePert[1]);
  printf("Muscle Length Before Pert: %f\n",muscleLengthBeforePert[1]);
  printf("Muscle Length After Pert: %f\n",muscleLengthAfterPert[1]);
  printf("Gain is: %f\n",gain[1]);
  bias[0] = 1 - (gain[0] * muscleLengthAfterPert[0]); // old constant 0.6
  bias[1] = 1 - (gain[1] * muscleLengthBeforePert[1]); // old constant 0.6
  printf("Bias is: %f\n\n",bias[1]);
  realTimeController->encoderBias[0] = bias[0];
  realTimeController->encoderBias[1] = bias[1];
  realTimeController->encoderGain[0] = gain[0];
  realTimeController->encoderGain[1] = gain[1];
  
  //GGAIN = ggain;
  //TBIAS = tbias;

  muscleCalibratedFlag = 1;

  printf("Calibration finished. Press space to continue.\n");
  return 1;
}
expParadigmMuscleLengthCalibration::~expParadigmMuscleLengthCalibration()
{

}