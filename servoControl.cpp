//*******************************************************************
//This is a virtual class that calls functions from dynamixel.h
//Refer http://support.robotis.com/en/software/dynamixelsdk.htm

//Refer the RX-28 manual at:
//http://support.robotis.com/en/product/dynamixel/rx_series/rx-28.htm
//*******************************************************************
#include "servoControl.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "dynamixel\import\dynamixel.h"

servoControl::servoControl(int ID) {
    servoID = ID;
    //initialize the dynamixel servo SERIAL connection using the FTDI driver
    if( dxl_initialize(DEFAULT_PORTNUM, DEFAULT_BAUDNUM) == 0 )
	{
		printf( "servoControl: Failed to open USB2Dynamixel!\n" );
		//printf( "Press any key to terminate...\n" );
		//getch();
		//return 0;
	}
	    else
		    printf( "servoControl: Serial Port %d is open (%d BAUD)...\n", DEFAULT_PORTNUM, DEFAULT_BAUDNUM);

    //set servo ID and check if servo is alive
    //dxl_set_txpacket_id(servoID);
    servoPing();
    //commCheck();
    setCompliance(1);
    goDefault();
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

void servoControl::PrintCommStatus(int CommStatus)
{
	switch(CommStatus)
	{
	case COMM_TXFAIL:
		printf("servoControl: COMM_TXFAIL: Failed transmit instruction packet!\n");
		break;

	case COMM_TXERROR:
		printf("servoControl: COMM_TXERROR: Incorrect instruction packet!\n");
		break;

	case COMM_RXFAIL:
		printf("servoControl: COMM_RXFAIL: Failed get status packet from device!\n");
		break;

	case COMM_RXWAITING:
		printf("servoControl: COMM_RXWAITING: Now recieving status packet!\n");
		break;

	case COMM_RXTIMEOUT:
		printf("servoControl: COMM_RXTIMEOUT: There is no status packet!\n");
		break;

	case COMM_RXCORRUPT:
		printf("servoControl: COMM_RXCORRUPT: Incorrect status packet!\n");
		break;

	default:
		printf("servoControl: This is unknown error code!\n");
		break;
	}
}

// Print error bit of status packet
void servoControl::PrintErrorCode()
{
	if(dxl_get_rxpacket_error(ERRBIT_VOLTAGE) == 1)
		printf("servoControl: Input voltage error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_ANGLE) == 1)
		printf("servoControl: Angle limit error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERHEAT) == 1)
		printf("servoControl: Overheat error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_RANGE) == 1)
		printf("servoControl: Out of range error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_CHECKSUM) == 1)
		printf("servoControl: Checksum error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERLOAD) == 1)
		printf("servoControl: Overload error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_INSTRUCTION) == 1)
		printf("servoControl: Instruction code error!\n");
}

int servoControl::servoPing() {
    dxl_ping(servoID);
    int status = commCheck();
    if (servoID == BROADCAST_ID)
        printf("servoControl: Testing Broadcast connection to Servos: ");
        else
            printf("servoControl: Testing connection to Servo %d: ",servoID);
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

//public declarations
int servoControl::setPosition(int position) {
    int checkVal = 3443, bitPosition = 0;
    float temp;
    waitMoving();
    if (position < -150 || position > 270) {
        printf("Invalid Position %d. Value of Position must be between [-150, 150] degrees\n",position);
        return 0;
    }
    else {
        temp = (position + 150)*POS_STEP;
        bitPosition = ( fabs(temp-ceil(temp)) < fabs(temp-floor(temp)) ) ? abs(ceil(temp)) : abs(floor(temp));
        dxl_write_word(servoID, P_GOAL_POSITION_L, bitPosition);
        //Sleep(CONTROL_PERIOD);
        checkVal = dxl_read_word(servoID, P_GOAL_POSITION_L);
        if( COMM_RXSUCCESS == commCheck() ) {
            if(checkVal != bitPosition) {
                printf("servoControl: ERROR: device position register corrupt!\n");
                return 0;
            }
        }
        else {
            return 0;
        }
    }
    return 1;
}

int servoControl::setVelocity(int velocity) {
    int checkVal = 3443, bitVelocity = 0;
    float temp;
    waitMoving();
    if (velocity > 476 || velocity < 0) {
        printf("Invalid Velocity %d. Value of Velocity must be between [0, 476] degrees/sec\n",velocity);
        return 0;
    }
    else {
        temp = velocity*VEL_STEP;
        bitVelocity = ( (temp-ceil(temp)) < (temp-floor(temp)) ) ? abs(ceil(temp)) : abs(floor(temp));
        dxl_write_word(servoID, P_GOAL_SPEED_L, bitVelocity);
        //Sleep(CONTROL_PERIOD);
        checkVal = dxl_read_word(servoID, P_GOAL_SPEED_L);
        if( COMM_RXSUCCESS == commCheck() ) {
            if(checkVal != bitVelocity) {
                printf("servoControl: ERROR: device velocity register corrupt!\n");
                return 0;
            }
        }
        else {
            return 0;
        }
    }
    return 1;
}

int servoControl::setCompliance(int level) {
    const unsigned int sleepTime = 10;
    const int cSlope = 64;
    const int cMargin = 1;
    const int cPunch = 32;
    const int maxVal[5] = {cSlope, cSlope, cMargin, cMargin, cPunch}, defVal[5] = {32, 32, 1, 1, 32};
    const int addr[5] = {CW_COMPLIANCE_SLOPE, CCW_COMPLIANCE_SLOPE, CW_COMPLIANCE_MARGIN, CCW_COMPLIANCE_MARGIN, CURRENT_PUNCH_L};
    int checkVal, temp, i;
    waitMoving();
    switch(level) {
        case 1:
            for(i = 0; i < 5; i++) {
                (i==4) ? dxl_write_word(servoID, addr[i], maxVal[i]) : dxl_write_byte(servoID, addr[i], maxVal[i]);
                Sleep(sleepTime);
                checkVal = (i==4) ? dxl_read_word(servoID, addr[i]) : dxl_read_byte(servoID, addr[i]);
                if( COMM_RXSUCCESS == commCheck() ) {
                    if(checkVal != maxVal[i]) {
                        printf("servoControl: ERROR: servo compliance register number %d corrupt!\n", addr[i]);
                        return 0;
                    }
                }
                else {
                    return 0;
                }
            }
            break;
        default: //same as case 0
            for(i = 0; i < 4; i++) {
                dxl_write_byte(servoID, addr[i], defVal[i]);
                Sleep(sleepTime);
                checkVal = dxl_read_byte(servoID, addr[i]);
                if( COMM_RXSUCCESS == commCheck() ) {
                    if(checkVal != defVal[i]) {
                        printf("servoControl: ERROR: servo compliance register number %d corrupt!\n", addr[i]);
                        return 0;
                    }
                }
                else {
                    return 0;
                }
            }
            break;
    }
    return 1;
}

int servoControl::torqueON(void)
{
    int checkVal = -1;
    dxl_write_byte(servoID, TORQUE_SWITCH, 1);
    Sleep(10);
    checkVal = dxl_read_byte(servoID, TORQUE_SWITCH);
    if( COMM_RXSUCCESS == commCheck() ) {
        if(checkVal != 1) {
            printf("servoControl: ERROR: servo Torque Reqister (%d) corrupt!\n", TORQUE_SWITCH);
            return 0;
        }
    }
    else {
        return 0;
    }
    return 1;
}

int servoControl::torqueOFF(void)
{
    int checkVal = -1;
    waitMoving();
    dxl_write_byte(servoID, TORQUE_SWITCH, 0);
    checkVal = dxl_read_byte(servoID, TORQUE_SWITCH);
    if( COMM_RXSUCCESS == commCheck() ) {
        if(checkVal != 0) {
            printf("servoControl: ERROR: servo Torque Reqister (%d) corrupt!\n", TORQUE_SWITCH);
            return 0;
        }
    }
    else {
        return 0;
    }
    return 1;
}

int servoControl::isTorque(void)
{
    int checkVal = -1;
    checkVal = dxl_read_byte(servoID, TORQUE_SWITCH);
    if( COMM_RXSUCCESS == commCheck() ) {
        if(checkVal != 0) {
            printf("servoControl: ERROR: servo Torque Reqister (%d) corrupt!\n", TORQUE_SWITCH);
            return -1;
        }
    }
    else {
        return -1;
    }
    return checkVal;
}

servoControl::~servoControl() {
    waitMoving();
    goDefault();
    torqueOFF();
    dxl_terminate();
}

int servoControl::isMoving() {
    int Moving =  (dxl_read_byte( servoID, P_MOVING ));
    return Moving;
}

void servoControl::waitMoving(int overRide) { //change overRide value in the header file to stop waiting
    int Moving = 1;
    if(overRide == 0) {
        do {
            if (0 == isMoving()) Moving = 0;
                else Sleep(CONTROL_PERIOD);
        }while (Moving == 1);
    }
}


//suraj - revisit
void servoControl::goDefault() {
    waitMoving();
    //printf("goDefault: setting velocity\n");
    setVelocity(RESET_VELOCITY);
    Sleep(CONTROL_PERIOD);
    //printf("goDefault: setting position\n");
    flexPos     = -57;
    extPos      = -7;
    int centerPos   = (flexPos + extPos) / 2;
    printf("\n\nServo center position: %d degrees.\n\n", centerPos);
    //setPosition(-7);
    setPosition(centerPos); //flex: -78, extend: 14, center: -46
    //printf("goDefault: Motor in Default position\n");
}

void servoControl::rampHold() {
    setVelocity(rampVelocity);
    
    setPosition(initPos);
    waitMoving();
    Sleep(holdPeriod);
    
    setPosition(finalPos);
    waitMoving();
    Sleep(holdPeriod);    
}

void servoControl::setPerturbationParameters(int initPos, int finalPos, int rampVelocity, int holdPeriod) {
    this->initPos = initPos;
    this->finalPos = finalPos;
    this->rampVelocity = rampVelocity;
    this->holdPeriod = holdPeriod;
}


int servoControl::servoTwitch(int pos, int vel) {
    setVelocity(vel);
    setPosition(pos);
    Sleep(CONTROL_PERIOD);
    goDefault();
    return 1;
}
