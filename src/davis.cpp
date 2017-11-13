#include "davis.h"
// Global flag and method to handle interrupts
// ***INTEGRATION TESTED***
bool rotationFlag = false;
void isrRotation() {
    rotationFlag = true;
}

// ***TESTED***
DavisAnemometer::DavisAnemometer( unsigned long _SamplePeriod, int _WindVanePin, int _WindSpeedPin, int _WindVaneOffset, bool _debugMode) {
    WindVanePin = _WindVanePin;
    WindVaneOffset = _WindVaneOffset;
    WindSpeedPin = _WindSpeedPin;
    SamplePeriod = _SamplePeriod;
    debugMode = _debugMode;
    Rotations = 0;
    RotationsBufferSize = ROTATION_BUFFER_SIZE;
    pinMode(WindVanePin, INPUT);
    memset(RotationsBuffer, 0, RotationsBufferSize);
    attachInterrupt(digitalPinToInterrupt(WindSpeedPin), isrRotation, FALLING);
}

// ***TESTED***
int DavisAnemometer::getDirection() {
    if ( debugMode ) {
        return (int)float(random(85, 95));
    }
    int VaneValue = analogRead(WindVanePin);
    int Direction = map(VaneValue, 0, 1023, 0, 360);
    int CalDirection = Direction + WindVaneOffset;
    if(CalDirection > 360) CalDirection = CalDirection - 360;
    if(CalDirection < 0) CalDirection = CalDirection + 360;
    return CalDirection;
}

// private method to process an interrupt (triggered by flag)
// ***TESTED***
void DavisAnemometer::handleInterrupt() {
    rotationFlag = false;
    unsigned long currMillis = millis();
    if ((currMillis - ContactBounceTime) > 15 ) { // debounce the switch contact.
        ContactBounceTime = currMillis;
        updateRotationsBuffer(currMillis); // used for rolling average calculation
        Rotations++; // used by simple speed calculation
    }
}

// public method to run in main loop
// ***INTEGRATION TESTED***
void DavisAnemometer::update() {
    if (rotationFlag) { handleInterrupt(); }
    updateSimpleWindSpeed();
}

// Maintains an array of the times of the last n ticks. When a new tick
// occours, it deletes the last item, shuffles everything down and adds
//another. Used for rolling averages method.
// ***TESTED***
void DavisAnemometer::updateRotationsBuffer(unsigned long _val) {
    for (int i = RotationsBufferSize - 1; i > 0; i--) {
         RotationsBuffer[i] = RotationsBuffer[i-1];
    }
    RotationsBuffer[0] = _val;
}

// simple count of rotatations since last sample
// ***TESTED***
void DavisAnemometer::updateSimpleWindSpeed () {
    unsigned long currMillis = millis();
    if ((currMillis - LastSample) >= SamplePeriod) {
        LastSample = currMillis;
        // convert to knots
        float T = SamplePeriod/1000;
        SimpleWindSpeed = (float)Rotations * (ROTATIONS_CONVERSION/T);
        Rotations = 0;
    }
}

// getter for SimpleWindSpeed
// ***INTEGRATION TESTED***
float DavisAnemometer::getSimpleWindSpeed() {
    if ( debugMode ) {
        return float(random(12000, 18000)) / float(1000);
    }
    return SimpleWindSpeed;
}

// ***TESTED***
float DavisAnemometer::getRollingWindSpeed() {
    if ( debugMode ) {
        return float(random(12000, 18000)) / float(1000);
    }
    float T = SamplePeriod/1000;
    return (float)countRotations() * (ROTATIONS_CONVERSION/T);
}

// Count the ticks that have occoured in the last n milliseconds.
// Used for rolling averages method.
// ***TESTED***
int DavisAnemometer::countRotations() {
    int tickCount = 0;
    for (int i = 0; i < RotationsBufferSize; i++) {
        if ((millis() - RotationsBuffer[i]) <= SamplePeriod && RotationsBuffer[i] != 0) {
            tickCount++;
        } else {
            break;
        }
    }
    return tickCount;
}

void DavisAnemometer::print(HardwareSerial& serial) {
    serial.print("Millis: ");
    serial.print(millis());
    serial.print(" Direction: ");
    serial.print(getDirection());
    serial.print(" Simple Wind Speed: ");
    serial.print(getSimpleWindSpeed());
    serial.print(" Rolling Wind Speed: ");
    serial.print(getRollingWindSpeed());
    serial.print('\n');
}
