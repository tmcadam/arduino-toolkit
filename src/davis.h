#ifndef DAVIS_H
#define  DAVIS_H
#include <Arduino.h>

#define ROTATION_BUFFER_SIZE 150
//#define ROTATIONS_CONVERSION 1.955196 // KNOTS
//#define ROTATIONS_CONVERSION 2.25 // MPH
#define ROTATIONS_CONVERSION 1.00584 // MS


void isrRotation();

class DavisAnemometer {
private:
    int WindVanePin;
    int WindVaneOffset;
    int WindSpeedPin;
    bool debugMode;

    // needed for both speed calculations
    unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine
    void handleInterrupt();

    // some variables and functions for simple speed calculation
    unsigned long Rotations;
    unsigned long SamplePeriod;
    unsigned long LastSample;
    float SimpleWindSpeed;
    void updateSimpleWindSpeed();

    //some variables and functions for rolling wind speed calculation
    float RollingWindSpeed;
    void updateRotationsBuffer(unsigned long);
    int countRotations();
    int RotationsBufferSize; // Will work upto around 100 knots wind.
    unsigned long RotationsBuffer[ROTATION_BUFFER_SIZE];

public:
    DavisAnemometer(unsigned long, int, int, int, bool _debugMode = false);
    void update();
    void print(HardwareSerial&);
    int getDirection();
    float getSimpleWindSpeed();
    float getRollingWindSpeed();
};
#endif
