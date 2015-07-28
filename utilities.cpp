#include <utilities.h>
#include <conio.h>
#include <dataOneSample.h>
#include <motorControl.h>
#include <expParadigm.h>
//#include <UDPClient.h>



int proceedState(int *state)
{
    static dataOneSample loadCellOffsets;
    //static UdpClient udpclient;
    static motorControl motors;
    static expParadigm paradigm(loadCellOffsets.loadCell1,loadCellOffsets.loadCell2);
    switch(*state)
    {
    case MOTOR_STATE_INIT:
        printf("Motors Winding Up; Next stage is Open-Loop\n");
        //udpclient.sendMessageToServer("KKK");
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
        motors.motorController();
        printf("Closed-Loop ; Next stage is Run Paradigm and Sample\n");
        *state = MOTOR_STATE_CLOSED_LOOP;
        break;
    case MOTOR_STATE_CLOSED_LOOP:
        printf("Running Paradigm; Next stage is Shutting Down\n");
        paradigm.startParadigm();
        *state = MOTOR_STATE_RUN_PARADIGM;
        break;
    case MOTOR_STATE_RUN_PARADIGM:
        printf("Shutting Down\n");
        motors.motorDisable();
        *state = MOTOR_STATE_SHUTTING_DOWN;
        printf("Press Enter to Exit\n");
        exit(0);   // Exit The Program
        break;
    }
    return 0;
}

