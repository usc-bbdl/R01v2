#include <utilities.h>
#include <stdio.h>
#include <conio.h>
#include <dataOneSample.h>
#include <motorControl.h>
#include <expParadigm.h>
#include <FPGAControl.h>

//float GGAIN = 0.0125; //default is (0.9/1000) //0.4/2000 is safe
float GGAIN = 0.0011;//0.005; //default is (0.9/1000) //0.4/2000 is safe
float TBIAS = 3;

int proceedState(int *state)
{
    static dataOneSample loadCellOffsets;
    static motorControl motors(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    static expParadigmMuscleLengthCalibration paradigmMuscleLengthCalibration(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    static expParadigmServoPerturbation paradigmServoPerturbation(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    static expParadigmManualPerturbation paradigmManualPerturbation(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    static expParadigmVoluntaryMovement paradigmVoluntaryMovement(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    switch(*state)
    {
    case STATE_INIT:
        printf("Motors Winding Up; Next stage is Open-Loop\n");
        motors.motorEnable();
        motors.motorWindUp();
        *state = STATE_WINDING_UP;
        break;
    case STATE_WINDING_UP:
        //Start Neural FPGA and Feeding muscle length data
        static FPGAControl bicepFPGA(BICEP,&motors);
        static FPGAControl tricepFPGA(TRICEP,&motors);
        *state = STATE_OPEN_LOOP;
        printf("Open-Loop ; Next stage is Closed-Loop\n");
        break;
    case STATE_OPEN_LOOP:
        //Connect the Neural FPGA and DAQ, Start controlling muscle force
        motors.motorControllerStart();
        Sleep(1000);
        motors.resetMuscleLength = TRUE;
        printf("Closed-Loop ; Next stage is Experiment Paradigm\n");
        *state = STATE_CLOSED_LOOP;
        break;
    case STATE_CLOSED_LOOP:
        int menu = 0;
        printf("Which Paradigm to run?\n");
        printf("- [1] Muscle Length Calibration\n");
        printf("- [2] Servo Perturbation\n");
        printf("- [3] Manual Perturbation\n");
        printf("- [4] Voluntary Movement\n");
        do{
            scanf("%d", &menu);
            if ((menu>4) || (menu<1))
                printf("Wrong input! try Again.\n");
        }while ((menu>4) || (menu<1));
        switch(menu)
        {
        case 1:
            *state = STATE_PARADIGM_LENGTH_CALIBRATION;
            printf("Muscle Length Calibration Selected\n");
            break;
       case 2:
            *state = STATE_RUN_PARADIGM_SERVO_PERTURBATION;
            printf("Servo Perturbation Selected\n");
            break;
        case 3:
            *state = STATE_RUN_PARADIGM_MANUAL_PERTURBATION;
            printf("Servo Perturbation Selected\n");
            break;
        case 4:
            *state = STATE_RUN_PARADIGM_VOLUNTARY_MOVEMENT;
            printf("Voluntary Movement Selected\n");
            break;
        }
//        Sleep(500);
//        paradigm.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        break;
    case STATE_PARADIGM_LENGTH_CALIBRATION:
        paradigmMuscleLengthCalibration.startParadigm(&motors);
        *state = STATE_CLOSED_LOOP;
        break;
    case STATE_RUN_PARADIGM_SERVO_PERTURBATION:
        paradigmServoPerturbation.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        *state = STATE_CLOSED_LOOP;
        break;
    case STATE_RUN_PARADIGM_MANUAL_PERTURBATION:
        paradigmManualPerturbation.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        *state = STATE_CLOSED_LOOP;
        break;
    case STATE_RUN_PARADIGM_VOLUNTARY_MOVEMENT:
        paradigmVoluntaryMovement.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        *state = STATE_CLOSED_LOOP;
        break;
    case STATE_SHUTTING_DOWN:
        printf("Shutting Down\n");
        motors.motorControllerEnd();
        printf("Press Enter to Exit\n");
        exit(0);
        break;
    }
    return 0;
}



int ReInterpret(float32 in, int32 *out)
{
    memcpy(out, &in, sizeof(int32));
    return 0;
}

int ReInterpret(int32 in, int32 *out)
{
    memcpy(out, &in, sizeof(int32));
    return 0;
}

int ReInterpret(int in, float *out)
{
    memcpy(out, &in, sizeof(float));
    return 0;
}