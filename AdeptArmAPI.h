#pragma once

#ifndef ADEPTARMAPI_H
#define ADEPTARMAPI_H

#include <string>
#include <winsock.h>
#include <windows.h>

struct PPoint
{
  long double x;
  long double y;
  long double z;
  long double a;
  long double b;
  long double c;
  
  PPoint() {  
        this->x = 0;
    this->y = -100;
    this->z = 10;
    this->a = 0;
    this->b = -5;
    this->c = 0;
  }
  
    PPoint(int signal) {
        if (signal == 0) { // Predefined point for connecting to cat limb
            this->x = -138.50;   // Max is 170
      this->y = -75.60;//-102;
      this->z = 186.91;//20;
      this->a = -90.62;//0;
      this->b = 88.65;//-13;
      this->c = 51.430;//42;
        }
        else if (signal == 1) { // Default position
            this->x = 0;   // Max is 170
      this->y = -60;
      this->z = 20;
      this->a = 0;
      this->b = -9;
      this->c = 42;
        }
    }

  PPoint(long double x, long double y, long double z, long double a, long double b, long double c) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->a = a;
    this->b = b;
    this->c = c;
  }

  std::string toString() {
    return std::to_string(x) + ","
                + std::to_string(y) + ","
                + std::to_string(z) + ","
                + std::to_string(a) + ","
                + std::to_string(b) + ","
                + std::to_string(c);
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