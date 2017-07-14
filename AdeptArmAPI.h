#pragma once

#ifndef ADEPTARMAPI_H
#define ADEPTARMAPI_H

#include <string>
#include <winsock.h>
#include <windows.h>

struct EPoint
{
    long double X;
    long double Y;
    long double Z;
    long double y;
    long double p;
    long double r;

    EPoint() {
        this->X = -505;
        this->Y = 199;
        this->Z = 279;
        this->y = -93;
        this->p = 93;
        this->r = 103;
    }

    EPoint(long double X, long double Y, long double Z, long double y, long double p, long double r) {
    this->X = X;
    this->Y = Y;
    this->Z = Z;
    this->y = y;
    this->p = p;
    this->r = r;
    }

    std::string toString() {
    return std::to_string(X) + ","
                + std::to_string(Y) + ","
                + std::to_string(Z) + ","
                + std::to_string(y) + ","
                + std::to_string(p) + ","
                + std::to_string(r);
    }
};

struct PPoint
{
    long double J1;
    long double J2;
    long double J3;
    long double J4;
    long double J5;
    long double J6;
  
  PPoint() {  
    this->J1 = 0;
    this->J2 = -100;
    this->J3 = 10;
    this->J4 = 0;
    this->J5 = -5;
    this->J6 = 0;
  }
  
    PPoint(int signal) {
        if (signal == 0) { // Predefined point for connecting to cat limb
            this->J1 = -138.50;   // Max is 170
            this->J2 = -75.60;//-102;
            this->J3 = 186.91;//20;
            this->J4 = -90.62;//0;
            this->J5 = 88.65;//-13;
            this->J6 = 51.430;//42;
        }
        else if (signal == 1) { // Default position
            this->J1 = 0;   // Max is 170
            this->J2 = -60;
            this->J3 = 20;
            this->J4 = 0;
            this->J5 = -9;
            this->J6 = 42;
        }
    }

  PPoint(long double J1, long double J2, long double J3, long double J4, long double J5, long double J6) {
    this->J1 = J1;
    this->J2 = J2;
    this->J3 = J3;
    this->J4 = J4;
    this->J5 = J5;
    this->J6 = J6;
  }

  std::string toString() {
    return std::to_string(J1) + ","
                + std::to_string(J2) + ","
                + std::to_string(J3) + ","
                + std::to_string(J4) + ","
                + std::to_string(J5) + ","
                + std::to_string(J6);
  }
};


enum VelocityType {
  MONITOR,
  INCHES_PER_SECOND,
  MILLIMETERS_PER_SECOND
};

class AdeptArmAPI
{
public:
    // Constructors
  AdeptArmAPI();
  AdeptArmAPI(int);
  AdeptArmAPI(std::string, int);
  
    // Network related
    SOCKET ConnectSocket;
  bool connectToController();
  bool disconnectFromServer();

    // Action functions
    void move(PPoint position);
    void moveE(EPoint position);
    void movetrans(PPoint position);
  void moveToOrigin();
  void moveInCircle(double startAngle, double endAngle, double radius, int numberOfPoints);
  void moveInCircleTrans(double radius, int numberOfPoints);
    //void moveToRandomPointOnCircle(double radius);

  // Set functions
    void setVelocity(long double velocity, long double rotationVelocity, VelocityType type, bool always);
    void setOrigin(PPoint position);
  void setBlocking(bool shouldBlock);

  // Helper functions
    void sendStr(std::string);

    PPoint origin;
    PPoint current;

private:
  std::string address;
  int portNumber;
  
  double velocity;
  double rotationalVelocity;
  bool blockingFlag;
    
};

#endif