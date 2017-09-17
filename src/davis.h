#ifndef DAVIS_H
#define  DAVIS_H
#include <Arduino.h>

#define ROTATION_BUFFER_SIZE 150

class DavisAnemometer {
private:
    int WindVanePin;
    int WindVaneOffset;
    int WindSpeedPin;

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
    int countRotations(unsigned long);
    int RotationsBufferSize; // Will work upto around 100 knots wind.
    unsigned long RotationsBuffer[ROTATION_BUFFER_SIZE];

public:
    DavisAnemometer(unsigned long, int, int, int _WindVaneOffset=0);
    void update();
    void print(HardwareSerial&);
    int getDirection();
    float getSimpleWindSpeed();
    float getRollingWindSpeed();
};
#endif
