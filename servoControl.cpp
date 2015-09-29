#include "servoControl.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "dynamixel\import\dynamixel.h" 

//private declarations
servoControl::servoControl(int ID) {
    servoID = ID;
    //initialize the dynamixel servo SERIAL connection using the FTDI driver
    if( dxl_initialize(DEFAULT_PORTNUM, DEFAULT_BAUDNUM) == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		//printf( "Press any key to terminate...\n" );
		//getch();
		//return 0;
	}
	    else
		    printf( "Serial Port %d is open (%d BAUD)...\n", DEFAULT_PORTNUM, DEFAULT_BAUDNUM);
    
    //set servo ID and check if servo is alive
    dxl_set_txpacket_id(servoID);
    servoPing();
    commCheck();
}

servoControl::servoControl() {
    servoID = 1;
    //initialize the dynamixel servo SERIAL connection using the FTDI driver
    if( dxl_initialize(DEFAULT_PORTNUM, DEFAULT_BAUDNUM) == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		//printf( "Press any key to terminate...\n" );
		//getch();
		//return 0;
	}
	    else
		    printf( "Serial Port %d is open (%d BAUD)...\n", DEFAULT_PORTNUM, DEFAULT_BAUDNUM);
    
    //set servo ID and check if servo is alive
    dxl_set_txpacket_id(servoID);
    servoPing();
    commCheck();
}

int servoControl::commCheck() {

    CommStatus = dxl_get_result();
	if( CommStatus == COMM_RXSUCCESS )
	{
		PrintErrorCode();
	}
	    else
	    {
		    PrintCommStatus(CommStatus);
	    }
    return CommStatus;
}

int servoControl::servoPing() {
    dxl_ping(servoID);
    int status = commCheck();
    if (servoID == BROADCAST_ID)
        printf("Testing Broadcast connection to Servos: ");
        else
            printf("Testing connection to Servo %d: ",servoID);
    if (status == COMM_RXSUCCESS) {
        printf("Ping success!\n");
        return 1;
    }
    return 0;
}

void servoControl::formatCMD(int position, int velocity) {  //function is a work in progress - do no call.
    dxl_set_txpacket_id(servoID);
    dxl_set_txpacket_instruction(INST_REG_WRITE);
    dxl_set_txpacket_parameter(0, P_GOAL_POSITION_L);
    dxl_set_txpacket_parameter(1, 2);

    //execute formatted command on Motor
    dxl_set_txpacket_instruction(INST_ACTION);
    dxl_tx_packet( );
}

void servoControl::setPosition(int position) {
    while(!isMoving());
    dxl_write_word(servoID, P_GOAL_POSITION_L, position);
}

void servoControl::setVelocity(int velocity) {
    while(!isMoving());
    dxl_write_word(servoID, P_GOAL_SPEED_L, velocity);
}

servoControl::~servoControl() {
    while(isMoving());
    dxl_terminate();
}

//public declarations
void servoControl::PrintCommStatus(int CommStatus)
{
	switch(CommStatus)
	{
	case COMM_TXFAIL:
		printf("COMM_TXFAIL: Failed transmit instruction packet!\n");
		break;

	case COMM_TXERROR:
		printf("COMM_TXERROR: Incorrect instruction packet!\n");
		break;

	case COMM_RXFAIL:
		printf("COMM_RXFAIL: Failed get status packet from device!\n");
		break;

	case COMM_RXWAITING:
		printf("COMM_RXWAITING: Now recieving status packet!\n");
		break;

	case COMM_RXTIMEOUT:
		printf("COMM_RXTIMEOUT: There is no status packet!\n");
		break;

	case COMM_RXCORRUPT:
		printf("COMM_RXCORRUPT: Incorrect status packet!\n");
		break;

	default:
		printf("This is unknown error code!\n");
		break;
	}
}

// Print error bit of status packet
void servoControl::PrintErrorCode()
{
	if(dxl_get_rxpacket_error(ERRBIT_VOLTAGE) == 1)
		printf("Input voltage error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_ANGLE) == 1)
		printf("Angle limit error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERHEAT) == 1)
		printf("Overheat error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_RANGE) == 1)
		printf("Out of range error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_CHECKSUM) == 1)
		printf("Checksum error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERLOAD) == 1)
		printf("Overload error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_INSTRUCTION) == 1)
		printf("Instruction code error!\n");	
}

int servoControl::isMoving() {
    return dxl_read_byte( servoID, P_MOVING );
}

void servoControl::goDefault() {
    while(isMoving());
    setVelocity(0);
    setPosition(512);
}

int servoControl::servoTwitch(int pos, int vel) {
    setVelocity(vel);
    setPosition(pos);
    sleep(CONTROL_PERIOD);
    goDefault();
    return 1;
}
