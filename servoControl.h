//*************************************************************
//This is a virtual class that calls functions from dynamixel.h
//Refer http://support.robotis.com/en/software/dynamixelsdk.htm
//*************************************************************

// Windows version

#pragma comment(lib, "dynamixel\\import\\dynamixel.lib")

#ifndef SERVOCONTROL_H
    #define SERVOCONTROL_H
    #include <windows.h>
    #include <math.h>
    #include <stdio.h>
    #include <conio.h>
    #include "dynamixel\import\dynamixel.h"  
    #define PI  3.141592f
    
    // Control addresses
    #define P_GOAL_POSITION_L	30
    #define P_GOAL_POSITION_H	31
    #define P_GOAL_SPEED_L		32
    #define P_GOAL_SPEED_H		33
    
    // Default setting
    #define DEFAULT_PORTNUM		3 // COM3
    #define DEFAULT_BAUDNUM		34 //157200 BAUD
		// Address 4   Set BPS      Goal BPS    Error
		// 1           1000000.0  	1000000.0  	 0.000 %
		// 3           500000.0  	500000.0  	 0.000 %
		// 4           400000.0  	400000.0  	 0.000 %
		// 7           250000.0  	250000.0  	 0.000 %
		// 9           200000.0  	200000.0  	 0.000 %
		// 16          117647.1	    115200.0    -2.124 %
		// 34          57142.9	    57600.0  	 0.794 %
		// 103         19230.8	    19200.0  	-0.160 %
		// 207         9615.4	    9600.0  	-0.160 %

    #define NUM_ACTUATOR		3 // Number of actuator
    #define STEP_THETA			(PI / 100.0f) // Large value is more fast
    #define CONTROL_PERIOD		(10) // msec (Large value is more slow)
    #define DEFAULT_ID			1
    #define DEFAULT_POS			1
    #define P_MOVING			46


    class servoControl {
        int servoID, CommStatus;
        
        void PrintCommStatus(int);
        void PrintErrorCode();
        void formatCMD(int, int); // WARNING: function is a work in progress - do no call.
        int initPos, finalPos, rampVelocity, holdPeriod; 
    
    public: void setPosition(int);
            void setVelocity(int);
            ~servoControl();
            servoControl(int ID = 1);
            int commCheck();
            int servoPing();
            int isMoving();
            void waitMoving(int overRide = 0);
            void goDefault(int defPos = 365);
            //void rampHold(int initPos = 368, int finalPos = 614, int rampVelocity = 1023, int holdPeriod = 1500); //368, 614, 1023, 2000
            void rampHold(); //368, 614, 1023, 2000
            int servoTwitch(int, int);
            void setPerturbationParameters(int, int , int , int);
    };

#endif

//Some more definitions
// Value           Meaning
// COMM_TXSUCCESS  Success of Instruction packet Transmission
// COMM_RXSUCCESS  Success of Status Packet Reception
// COMM_TXFAIL     Failure of Instruction Packet Transmission by Error
// COMM_RXFAIL     Failure of Status Packet Reception by Error
// COMM_TXERROR    Problems in Instruction packet
// COMM_RXWAITING  Status packet is not arrived yet.
// COMM_RXTIMEOUT  Relevant Dynamixel is not responding.
// COMM_RXCORRUPT  Problems in Status packet


// Value	Name			Contents
// 1		INST_PING		No execution. It is used when controller is ready to receive Status Packet
// 2		INST_READ		This command reads data from Dynamixel
// 3		INST_WRITE		This command writes data to Dynamixel
// 4		INST_REG_WRITE	It is similar to WRITE_DATA, but it remains in the standby state without being executed until the ACTION command arrives.
// 5		INST_ACTION		This command initiates motions registered with REG WRITE
// 6		INST_RESET		This command restores the state of Dynamixel to the factory default setting.
// 131		INST_SYNC_WRITE	This command is used to control several Dynamixels simultaneously at a time.
