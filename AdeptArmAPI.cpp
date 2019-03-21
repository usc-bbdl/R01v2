// AdeptArmAPI.cpp
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <ws2tcpip.h>
#include "adeptArmAPI.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <io.h>
#include <regex>
#include <random>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define PI 3.14159265

std::string adeptIP = "172.16.184.121";
int adeptPort = 1234;

// Error printing helper function
void error(const char *msg){
    perror(msg);
    exit(0);
}
        
// Constructors 
// Constructor for default(localhost:1234)
AdeptArmAPI::AdeptArmAPI () {
    address = adeptIP;
    portNumber = adeptPort;
	blockingFlag = true;
    current = PPoint(0, 0, 0, 0, 0, 0);
}
// Constructor for localhost:givenPortNumber
AdeptArmAPI::AdeptArmAPI (int inputPortNumber) {
    address = adeptIP;
    portNumber = inputPortNumber;
	blockingFlag = true;
    current = PPoint(0, 0, 0, 0, 0, 0);
}
// Constructor for givenAddress:givenPortNumber
AdeptArmAPI::AdeptArmAPI (std::string address, int inputPortNumber) {
    address = address;
    portNumber = inputPortNumber;
	blockingFlag = true;
    current = PPoint(0, 0, 0, 0, 0, 0);
}

// Function to start connection and initialize buffer
bool AdeptArmAPI::connectToController() {
    WSADATA wsaData;
    ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                      *ptr = NULL,
                      hints;

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    char* portNumberBuf = new char[6];
    sprintf(portNumberBuf, "%d", portNumber);
    
    // Resolve the server address and port
    iResult = getaddrinfo(address.c_str(), portNumberBuf, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                                ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return false;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }
    return true;
}

bool AdeptArmAPI::disconnectFromServer () {
    // send a empty string as the disconnection signal to server  
    std::string disconnectSignal = "0";
    const char* sendbuf = disconnectSignal.c_str();
    int iResult = send(ConnectSocket, sendbuf, 1, 0 );
    
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }
    return true;
}

// Function Move
void AdeptArmAPI::move (PPoint position) {
    std::string move_command_string = "1,";
    move_command_string.append(position.toString());
    move_command_string.append("!");

    sendStr(move_command_string);
}

// Function Move
void AdeptArmAPI::movetrans (PPoint displacement) {
    std::string move_command_string = "5,";
    move_command_string.append(displacement.toString());
    move_command_string.append("!");
    
    sendStr(move_command_string);
    current.x = current.x + displacement.x;
    current.y = current.y + displacement.y;
    current.z = current.z + displacement.z;
}

void AdeptArmAPI::moveInCircle(double startAngle, double endAngle, double radius, int numberOfPoints) {	
	std::string commandString = "4,";
	commandString.append(std::to_string((long double)startAngle) + ",");
	commandString.append(std::to_string((long double)endAngle) + ",");
	commandString.append(std::to_string((long double)radius) + ",");
	commandString.append(std::to_string((long double)numberOfPoints) + "!");
	sendStr(commandString);
}

void AdeptArmAPI::moveInCircleTrans(double radius, int numberOfPoints) {

    // Move to start position
    //movetrans(PPoint(radius, 0, 0, 0, 0, 0));
    
    long double displacementAngle = 180 / numberOfPoints;
    
    for (int i = 0; i <= numberOfPoints; i++) {
        std::cout << "This is the " << i << "th loop" << std::endl;

        long double displacementFromDefaultX = cos((displacementAngle*i)*PI/180) * radius - current.x;
        long double displacementFromDefaultY = -(sin((displacementAngle*i)*PI/180) * radius) - current.y;
        
        std::cout << "Current: " << current.x << "," << current.y << std::endl;
        std::cout << "This point " << displacementFromDefaultX << "|" << displacementFromDefaultY << std::endl;
        movetrans(PPoint(displacementFromDefaultX, displacementFromDefaultY, 0, 0, 0, 0));
    }

    move(PPoint(0));
}

/*
void AdeptArmAPI::moveToRandomPointOnCircle(double radius) {
    
    double angle = distribution(generator);
    std::cout << "Moving to angle " << angle << std::endl;
    long double displacementFromDefaultX = cos(angle*PI/180) * radius - current.x;
    long double displacementFromDefaultY = -(sin(angle*PI/180) * radius - current.y);
    std::cout << "This point " << displacementFromDefaultX + current.x << "|" << displacementFromDefaultY + current.y << std::endl;
    std::cout << "This vector " << displacementFromDefaultX << "|" << displacementFromDefaultY << std::endl;
    movetrans(PPoint(displacementFromDefaultX, displacementFromDefaultY, 0, 0, 0, 0));
}
*/

// Function set velocity
void AdeptArmAPI::setVelocity (long double velocity, long double rotationVelocity, VelocityType type, bool always) {
    this->velocity = velocity;
	this->rotationalVelocity = rotationVelocity;
	std::string set_velocity_command_string = "2," + std::to_string(velocity) + "," + std::to_string(rotationVelocity) + ",";

	switch (type) {
	case MONITOR :
		set_velocity_command_string.append("0,");
		break;
	case INCHES_PER_SECOND :
		set_velocity_command_string.append("1,");
		break;
	case MILLIMETERS_PER_SECOND :
		set_velocity_command_string.append("2,");
		break;
	default :
		break;
	}

	if (always) {
		set_velocity_command_string.append("0!");
	}
	else {
		set_velocity_command_string.append("1!");
	}

	sendStr(set_velocity_command_string);
}


// Function set origin
void AdeptArmAPI::setOrigin (PPoint position) {
    origin = position;
}

void AdeptArmAPI::moveToOrigin () {
	move(origin);
    current = PPoint(0,0,0,0,0,0);
}

void AdeptArmAPI::setBlocking(bool shouldBlock) {
	this->blockingFlag = shouldBlock;
	std::string command = "3,";
	std::string block = shouldBlock ? "1" : "0";
	command.append(block);
	sendStr(command);
}

void AdeptArmAPI::sendStr(std::string input) {
    const char* sendbuf = input.c_str();
                
    int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }
    //printf("Bytes Sent: %ld\n", iResult);
	
	if (blockingFlag) {
		char recvbuf[10];
		iResult = recv(ConnectSocket, recvbuf, 10, 0);
		if ( iResult > 0 ) {
			//printf("Bytes received: %d\n", iResult);
			//std::cout << "Received: " << recvbuf << std::endl;
		} else if ( iResult == 0) {
			//printf("No bytes received\n");
		} else {
			//printf("Recv failed with error: %d\n", WSAGetLastError());
		}
	}
}