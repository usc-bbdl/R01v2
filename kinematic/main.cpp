// Cadaver Experiment Project
// =============================================================
//
// Developers:
// John Rocamora	johnrocamora@gmail.com
// Victor Barradas	victor_barradas@hotmail.com
//
// =============================================================

#define WIN32_LEAN_AND_MEAN
#include "UdpClient.h"
#include "UdpServer.h"
#include "analogServer.h"
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include "include/freeglut.h"
#include "TimeData.h"	// Class for performance timer 
#include "LoadCellData.h"	// Class for load cell data
#include "DCMotorCommand.h"	// Class for DC motor voltage control
#include "EncoderData.h"	// Class for DC motor voltage control
#include "Controller.h"	// Class for DC motor voltage control
#include "KinematicPerturbation.h"

#ifdef _WIN64
#	pragma comment(lib, "lib/x64/freeglut.lib")
#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")
#else
#	pragma comment(lib, "lib/x86/glut32.lib")
#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")
#endif

// Function Prototypes
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
int initializeProgram();
void terminateProgram();
int update();

int countServerName = 0;

// Global Variables
TimeData gPerformanceTimer; 
LoadCellData gLoadCellData = LoadCellData(
	"C:\\data\\%s\.dev_loadcells\.txt",
	"time,ch1,ch2,ch3,ch4,ch5,ch6,ch7\n"
	);
DCMotorCommand gDCMotorCommand;
EncoderData gEncoderData = EncoderData(
	"C:\\data\\%s\.dev_encoders\.txt",
	"time,ch1,ch2,ch3,ch4,ch5,ch6,ch7\n"
	);
Controller gController = Controller(&gLoadCellData, &gDCMotorCommand);
KinematicPerturbation gKinematicPerturbation;

UdpClient gUdpClient;
UdpServer gUdpServer;
analogServer gAnalogServer(&gLoadCellData);

bool bIsKinematic = false;
bool bIsPerturbing = false;
bool bIsRecording = false;
//Do you see the changes?
HINSTANCE hInst;
TCHAR ch = ' ';			// Character entered
FILE *fileConsole;

// Constants
const char CLASS_NAME[] = "WinMain";
const char APP_TITLE[] = "Hello World";	// Title bar text
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Control Constants
char gStatusString[11][300];

void outputText(int x, int y, char *strOut);
void renderConsole();
void renderConsole()
{
	outputText(0, 25, "Ch   LoadCell   Encoder   MotorCommand   Desired   Error");
	outputText(0, 0, "Time:");
	outputText(50, 0, gStatusString[0]);
	outputText(0, 50, gStatusString[1]);
	outputText(0, 100, gStatusString[2]);
	outputText(0, 150, gStatusString[3]);
	outputText(0, 200, gStatusString[4]);
	outputText(0, 250, gStatusString[5]);
	outputText(0, 300, gStatusString[6]);
	outputText(0, 350, gStatusString[7]);
	outputText(0, 400, gStatusString[8]);
	outputText(0, 450, gStatusString[9]);
	//outputText(0, 500, gStatusString[10]);

	if(gEncoderData.bIsRecording) {
		outputText(0, 500, "R_Encoders");
	}
	if(gLoadCellData.bIsRecording) {
		outputText(0, 550, "R_LoadCells");
	}
	if(gController.bForceControlOn) {
		outputText(200, 500, "ForceControl On");
	}
	//Kian starts here
	if(gController.bWindUp) {
		outputText(600, 200, "Wind Up");
	}
	if(gDCMotorCommand.bIsAmplifiersOn) {
		outputText(600, 300, "Amplifiers on");
	}
	//kian ends here
	if(bIsKinematic) {
		outputText(200, 550, "Kinematic");
	}
	else {
		outputText(200, 550, "Phantom");
	}
	if(bIsPerturbing) {
		outputText(400, 500, "Perturbing");
	}
	if(bIsRecording) {
		outputText(400, 600, "Recording");
	}
}

// Takes x and y with
//		The origin in the top-left corner of the window
//		x goint to the right and y going downwards
void outputText(int x, int y, char *strOut)
{
	int len;
	glRasterPos2f((float) x, (float) WINDOW_HEIGHT - y - 25);
	len = (int) strlen(strOut);
	{
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (UCHAR*)strOut);
	}
}

void idle(void)
{
	// Test the update function here and compare the data logging to 
	// determine if we are not constraints
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key) {
	case 27:
		exit(0);
		break;

	case 'C':	// Enable Force Control
	case 'c':
		gController.enableForceControl();
		break;

	case 'Q':	// "Motor 0 Up"
	case 'q':
		gController.updateDesiredReading(0, gController.desiredReading[0] + 0.001);
		break;

	case 'A':	// "Motor 0 Down"
	case 'a':
		gController.updateDesiredReading(0, gController.desiredReading[0] - 0.001);
		break;

	case 'W':	// "Motor 2 Up"
	case 'w':
		gController.updateDesiredReading(2, gController.desiredReading[2] + 0.01);
		break;

	case 'S':	// "Motor 2 Down"
	case 's':
		gController.updateDesiredReading(2, gController.desiredReading[2] - 0.01);
		break;

	case 'E':	// "Motor 4 Up"
	case 'e':
		gController.updateDesiredReading(4, gController.desiredReading[4] + 0.01);
		break;

	case 'D':	// "Motor 4 Down"
	case 'd':
		gController.updateDesiredReading(4, gController.desiredReading[4] - 0.01);
		break;

	case 'R':	// "Motor 5 Up"
	case 'r':
		gController.updateDesiredReading(5, gController.desiredReading[5] + 0.001);
		break;

	case 'F':	// "Motor 5 Down"
	case 'f':
		gController.updateDesiredReading(5, gController.desiredReading[5] - 0.001);
		break;

	case 'T':	// "Motor 6 Up"
	case 't':
		gController.updateDesiredReading(4, gController.desiredReading[6] + 0.001);
		break;

	case 'G':	// "Motor 6 Down"
	case 'g':
		gController.updateDesiredReading(4, gController.desiredReading[6] - 0.001);
		break;

	case 'L':	// "Logging Generic"
	case 'l':
		gEncoderData.setFileName("encoders");
		gLoadCellData.setFileName("loadcells");
		gEncoderData.timeData.resetTimer();
		gLoadCellData.timeData.resetTimer();
		gEncoderData.startRecording();
		gLoadCellData.startRecording();
		break;

	case 'N':
	case 'n':
		gDCMotorCommand.TurnAmplifiersOn();
		break;


	case VK_SPACE:
		gDCMotorCommand.TurnAmplifiersOff();
		gController.bForceControlOn = false;
		gController.bWindUp = false;
		break;

	case 'P':	// "Pull"
	case 'p':
		//by kian		gController.bWindUp = true;
		gController.enableWindUp();
		//gDCMotorCommand.WindUpMotors();
		break;

	case 'Z':
	case 'z':
		gKinematicPerturbation.ReadPerturbationFile();
		gKinematicPerturbation.TrackPerturbationFromFile();
	
		//gKinematicPerturbation.SineWavePerturbationTimer();
		//gKinematicPerturbation.RampHoldandBack();
		break;
	case 'B':
	case 'b':
		gEncoderData.closeRecordingFile();
		gLoadCellData.closeRecordingFile();
		break;


	default:
		break;
	}
}

//
//int update() 
//{
//	if(gUdpServer.bReceivedName) {
//		gEncoderData.setFileName(gUdpServer.sNameOfDataFile);
//		gLoadCellData.setFileName(gUdpServer.sNameOfDataFile);
//		if(bIsKinematic) {
//			gKinematicPerturbation.setTimeStamp(gUdpServer.sNameOfDataFile);
//		}
//		else {
//			//gUdpClient.sendMessageToServer(gUdpServer.sNameOfDataFile);
//		}
//		gUdpServer.bReceivedName = false;
//	}
//	if(gUdpServer.bReceivedKKK) {
//		//bIsKinematic = !bIsKinematic;
//		bIsKinematic = true;
//		gUdpServer.bReceivedKKK = false;
//	}
//	if(gUdpServer.bReceivedRRR) {
//		gEncoderData.timeData.resetTimer();
//		gLoadCellData.timeData.resetTimer();
//		gKinematicPerturbation.timeData.resetTimer();
//		bIsRecording = true;
//		gEncoderData.startRecording();
//		gLoadCellData.startRecording();
//		if(bIsKinematic) {
//			gKinematicPerturbation.startRecording();	
//		}
//		else {
//			//gUdpClient.sendMessageToServer("RRR");
//		}
//		gUdpServer.bReceivedRRR = false;
//	}
//	if(gUdpServer.bReceivedPPP) {
//		bIsPerturbing = true;
//		if(bIsKinematic) {
//			//gKinematicPerturbation.SineWavePerturbationLookUpTable();
//			//gKinematicPerturbation.RampHoldandBack();
//			gKinematicPerturbation.ReadPerturbationFile();
//			gKinematicPerturbation.TrackPerturbationFromFile();
//		}
//		else {
//			gUdpClient.sendMessageToServer("PPP");
//		}
//		gUdpServer.bReceivedPPP = false;
//	}
//	if(gUdpServer.bReceivedTTT) {
//		bIsPerturbing = false;
//		bIsRecording = false;
//		gEncoderData.closeRecordingFile();
//		gLoadCellData.closeRecordingFile();
//		if(bIsKinematic) {
//			gKinematicPerturbation.stopRecording();
//			gKinematicPerturbation.StopPerturbing();
//		}
//		else {
//			//gUdpClient.sendMessageToServer("TTT");
//		}
//		gUdpServer.bReceivedTTT = false;
//	}
//
//	return 0;
//}

int update() 
{
	if(gAnalogServer.bReceivedName) {
		gEncoderData.setFileName(gAnalogServer.sNameOfDataFile);
		gLoadCellData.setFileName(gAnalogServer.sNameOfDataFile);
		if(bIsKinematic) {
			gKinematicPerturbation.setTimeStamp(gAnalogServer.sNameOfDataFile);
		}
		gAnalogServer.bReceivedName = false;
	}
	if(gAnalogServer.bReceivedKKK) {
		bIsKinematic = true;
		gAnalogServer.bReceivedKKK = false;
	}
	if(gAnalogServer.bReceivedRRR) {
		gEncoderData.timeData.resetTimer();
		gLoadCellData.timeData.resetTimer();
		gKinematicPerturbation.timeData.resetTimer();
		bIsRecording = true;
		gEncoderData.startRecording();
		gLoadCellData.startRecording();
		gAnalogServer.bReceivedRRR = false;
		if(bIsKinematic) 
			gKinematicPerturbation.startRecording();	
	}
	if(gAnalogServer.bReceivedPPP) {
		//Sleep(500);
		bIsPerturbing = true;
		bIsKinematic = true;
		gKinematicPerturbation.RampHoldandBack();
		gAnalogServer.bReceivedPPP = false;
		//if(bIsKinematic) {
			//gKinematicPerturbation.SineWavePerturbationLookUpTable();
			
			//gKinematicPerturbation.ReadPerturbationFile();
			//gKinematicPerturbation.TrackPerturbationFromFile();
			
		//}
	}
	if(gAnalogServer.bReceivedTTT) {
		bIsPerturbing = false;
		bIsRecording = false;
		gEncoderData.closeRecordingFile();
		gLoadCellData.closeRecordingFile();
		if(bIsKinematic) {
			gKinematicPerturbation.stopRecording();
			gKinematicPerturbation.StopPerturbing();
		}
		gAnalogServer.bReceivedTTT = false;
	}
	if(gAnalogServer.bNotConnected) {
		bIsPerturbing = false;
		bIsRecording = false;
		bIsKinematic = false;
		gEncoderData.closeRecordingFile();
		gLoadCellData.closeRecordingFile();
		if(bIsKinematic) {
			gKinematicPerturbation.stopRecording();
			gKinematicPerturbation.StopPerturbing();
		}
		gAnalogServer.bNotConnected = false;
	}
	return 0;
}

// Callback function called by GLUT to render screen
void Display()
{
	update();

	// Clear frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0.5f, 0.5f, 0.5f);

	sprintf_s(gStatusString[0], 
		"%.1f", 
		gPerformanceTimer.getCurrentTime()
	);
	sprintf_s(gStatusString[1], 
		"0         %.3f       %05d             %.3f               %.3f      %.3f\n", 
		gLoadCellData.outDisplayValues[0], 
		gEncoderData.outDisplayValuesULONG[0], 
		gDCMotorCommand.motorVoltages[1], 
		gController.desiredReading[0], 
		gController.errorReading[0]);
	sprintf_s(gStatusString[2], 
		"2         %.3f       %05d             %.3f               %.3f      %.3f\n", 
		gLoadCellData.outDisplayValues[2],
		gEncoderData.outDisplayValuesULONG[2],
		gDCMotorCommand.motorVoltages[2],
		gController.desiredReading[2],
		gController.errorReading[2]);

	sprintf_s(gStatusString[3], 
		"4         %.3f       %05d             %.3f               %.3f      %.3f\n", 
		gLoadCellData.outDisplayValues[4], 
		gEncoderData.outDisplayValuesULONG[4], 
		gDCMotorCommand.motorVoltages[3],  
		gController.desiredReading[4], 
		gController.errorReading[4]);

	sprintf_s(gStatusString[4], 
		"5         %.3f       %05d             %.3f               %.3f      %.3f\n", 
		gLoadCellData.outDisplayValues[5], 
		gEncoderData.outDisplayValuesULONG[5], 
		gDCMotorCommand.motorVoltages[4],  
		gController.desiredReading[5], 
		gController.errorReading[5]);

	sprintf_s(gStatusString[5], 
		"6         %.3f       %05d             %.3f              %.3f      %.3f\n", 
		gLoadCellData.outDisplayValues[6], 
		gEncoderData.outDisplayValuesULONG[6], 
		gDCMotorCommand.motorVoltages[5],  
		gController.desiredReading[6], 
		gController.errorReading[6]);

	sprintf_s(gStatusString[6], 
		"A(1)     %.3f      %05d", 
		gLoadCellData.outDisplayValues[1],
		gEncoderData.outDisplayValuesULONG[1]
	);

	sprintf_s(gStatusString[7], 
		"B(2)     %.3f      %05d", 
		gLoadCellData.outDisplayValues[7],
		gEncoderData.outDisplayValuesULONG[3]
	);
	sprintf_s(gStatusString[10], 
		"analog Server Voltage: %.3f", 
		gAnalogServer.voltageValue
	);
	glColor3f(0.0f, 0.9f, 0.0f);
	renderConsole();




	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	// glutRedisplay();
}


// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0.0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Function called at init
int initializeProgram()
{
	return 0;
}

// Function called at exit
void terminateProgram()
{
}



//==================================================
// Starting point for a Windows application
// Parameters:
//		hInstance. Handle to the current instance of the application
//		hPrevInstance. Always NULL, obsolete parameter
//		lpCmdLine. Pointer to null-terminated string of command line args
//		nCmdShow. Specifies how the window is to be shown
//==================================================
int WINAPI WinMain( HINSTANCE	hInstance,
				   HINSTANCE	hPrevInstance,
				   LPSTR		lpCmdLine,
				   int			nCmdShow)
{
	MSG msg;
	// MessageBoxA(NULL, "akljrghjerk","ajkslg", MB_OK);

	// Create the window
	if (!CreateMainWindow(hInstance, nCmdShow)) {
		return false;
	}
	countServerName = 0;


	int argc = 1;
	char *argv[1] = {(char*)"Something"};

	float axis[] = { 0.7f, 0.7f, 0.0f }; // initial model rotation
	float angle = 0.8f;

	initializeProgram();

	// Initialize GLUT
	glutInit(&argc, argv);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Non-AntTweakBar simple example using GLUT");
	glutCreateMenu(NULL);

	// Set GLUT callbacks
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	atexit(terminateProgram);  // Called after glutMainLoop ends

	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	// Call the GLUT main loop
	glutMainLoop();

	return msg.wParam;
}

//==================================================
// Window event callback function
//==================================================
LRESULT WINAPI WinProc( HWND	hWnd,
					   UINT	msg,
					   WPARAM	wParam,
					   LPARAM	lParam)
{
	switch(msg) {
	case WM_DESTROY:
		// Tell Windows to kill this program
		PostQuitMessage(0);
		return 0;
	case WM_CHAR:
		switch(wParam) {
		case 27:
			PostQuitMessage(WM_QUIT);
			return 0;
		default:
			ch = (TCHAR) wParam;	// Get the character
			return 0;
		}
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam);
	}
}

//==================================================
// Create the window
// Returns: false on error
//==================================================
bool CreateMainWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hWnd;

	// Fill in the window class structure with parameters
	// that describe the main window
	wcx.cbSize = sizeof(wcx);				// Size of structure
	wcx.style = CS_HREDRAW | CS_VREDRAW;	// Redraw if size changes
	wcx.lpfnWndProc = WinProc;				// Points to window procedure
	wcx.cbClsExtra = 0;						// No extra class memory
	wcx.cbWndExtra = 0;						// Handle to instance
	wcx.hInstance = hInstance;				
	wcx.hIcon = NULL;						
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);	// Predefined arrow
	// Background brush
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcx.lpszMenuName = NULL;				// Name of menu resource
	wcx.lpszClassName = CLASS_NAME;			// Name of window class
	wcx.hIconSm = NULL;						// Small class icon

	// Register the window class
	// RegisterClassEx returns 0 on error
	if(RegisterClassEx(&wcx) == 0) {		// If error
		return false;
	}
	// Create window
	hWnd = CreateWindow(
		CLASS_NAME,				// Name of the window class
		APP_TITLE,				// Title bar text
		WS_OVERLAPPEDWINDOW,	// Window style
		CW_USEDEFAULT,			// Default horizontal position of window
		CW_USEDEFAULT,			// Defautl vertical position of window
		WINDOW_WIDTH,			// Width of window
		WINDOW_HEIGHT,			// Height of window
		(HWND) NULL,			// No parent window
		(HMENU) NULL,			// No menu
		hInstance,				// Handle to application instance
		(LPVOID) NULL);			// No window parameters

	// If there was an error creating the window
	if(!hWnd) {
		return false;
	}
	// ShowWindow(hWnd, nCmdShow);
	// Send a WM_PAINT message to the window procedure
	UpdateWindow(hWnd);

	return true;
}



