#include <utilities.h>
#include <conio.h>
#include <dataOneSample.h>
#include <motorControl.h>
#include <expParadigm.h>
#include <FPGAControl.h>

//float GGAIN = 0.0125; //default is (0.9/1000) //0.4/2000 is safe
float GGAIN = 0.005; //default is (0.9/1000) //0.4/2000 is safe
float TBIAS = 3;

int proceedState(int *state)
{
    static dataOneSample loadCellOffsets;
    static motorControl motors(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    static FPGAControl bicepFPGA(BICEP,&motors);
    static FPGAControl tricepFPGA(TRICEP,&motors);
    static expParadigm paradigm(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2,&ANALOG_Client);
    switch(*state)
    {
    case MOTOR_STATE_INIT:
        printf("Motors Winding Up; Next stage is Open-Loop\n");
        motors.motorEnable();
        motors.motorWindUp();
        *state = MOTOR_STATE_WINDING_UP;
        break;
    case MOTOR_STATE_WINDING_UP:
        //Start Neural FPGA and Feeding muscle length data
        
        *state = MOTOR_STATE_OPEN_LOOP;
        printf("Open-Loop ; Next stage is Closed-Loop\n");
        break;
    case MOTOR_STATE_OPEN_LOOP:
        //Start NI FPGA, Connect the Neural FPGA force command to the NI FPGA, Start controlling muscle force
        motors.motorControllerStart();
        Sleep(1000);
        motors.resetMuscleLength = TRUE;
        printf("Closed-Loop ; Next stage is Run Paradigm and Sample\n");
        *state = MOTOR_STATE_CLOSED_LOOP;
        break;
        /*
    case MOTOR_STATE_CLOSED_LOOP:
        //Start NI FPGA, Connect the Neural FPGA force command to the NI FPGA, Start controlling muscle force
        printf("Gamma updated. Running feeling experiment.\n");
        *state = MOTOR_STATE_GAMMA_UPDATED;
        break;
        */
    case MOTOR_STATE_CLOSED_LOOP:
        printf("Running Paradigm; Next stage is Shutting Down\n");
        //ANALOG_Client.sendMessageToServer(MESSAGE_PERTURB);
        Sleep(500);
        //ANALOG_Client.sendMessageToServer(MESSAGE_RECORD);
        paradigm.startParadigm(&bicepFPGA, &tricepFPGA, &motors);
        *state = MOTOR_STATE_RUN_PARADIGM;
        break;
    case MOTOR_STATE_RUN_PARADIGM:
        printf("Shutting Down\n");
        motors.motorControllerEnd();
        *state = MOTOR_STATE_SHUTTING_DOWN;
        printf("Press Enter to Exit\n");
        exit(0);   // Exit The Program
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