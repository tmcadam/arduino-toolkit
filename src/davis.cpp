#include "davis.h"

// Global flag and method to handle interrupts
bool rotationFlag = false;
void isrRotation() {
    rotationFlag = true;
}

DavisAnemometer::DavisAnemometer( unsigned long _SamplePeriod, int _WindVanePin, int _WindSpeedPin, int _WindVaneOffset) {
    WindVanePin = _WindVanePin;
    WindVaneOffset = _WindVaneOffset;
    WindSpeedPin = _WindSpeedPin;
    SamplePeriod = _SamplePeriod;
    RotationsBufferSize = ROTATION_BUFFER_SIZE;
    pinMode(WindVanePin, INPUT);
    memset(RotationsBuffer, 0, RotationsBufferSize);
    attachInterrupt(digitalPinToInterrupt(WindSpeedPin), isrRotation, FALLING);
}

int DavisAnemometer::getDirection() {
    int VaneValue = analogRead(WindVanePin);
    int Direction = map(VaneValue, 0, 1023, 0, 360);
    int CalDirection = Direction + WindVaneOffset;
    if(CalDirection > 360) CalDirection = CalDirection - 360;
    if(CalDirection < 0) CalDirection = CalDirection + 360;
    return CalDirection;
}

// private method to process an interrupt (triggered by flag)
void DavisAnemometer::handleInterrupt() {
    rotationFlag = false;
    unsigned long currMillis = millis();
    if ((currMillis - ContactBounceTime) > 15 ) { // debounce the switch contact.
        ContactBounceTime = currMillis;
        updateRotationsBuffer(ContactBounceTime); // used for rolling average calculation
        Rotations++; // used by simple speed calculation
    }
}

// public method to run in main loop
void DavisAnemometer::update() {
    if (rotationFlag) { handleInterrupt(); }
    updateSimpleWindSpeed();
}

// Maintains an array of the times of the last n ticks. When a new tick
// occours, it deletes the last item, shuffles everything down and adds
//another. Used for rolling averages method.
void DavisAnemometer::updateRotationsBuffer(unsigned long _val) {
    for (int i = RotationsBufferSize - 1; i > 0; i--) {
         RotationsBuffer[i] = RotationsBuffer[i-1];
    }
    RotationsBuffer[0] = _val;
}

// simple count of rotatations since last sample
void DavisAnemometer::updateSimpleWindSpeed () {
    unsigned long currMillis = millis();
    if ((currMillis - LastSample) > SamplePeriod) {
        LastSample = currMillis;
        // convert to knots
        float T = SamplePeriod/1000;
        SimpleWindSpeed = (float)Rotations * (1.955196/T); // V = P(2.25/T) * 0.868976
        Rotations = 0;
    }
}

float DavisAnemometer::getSimpleWindSpeed() {
    return SimpleWindSpeed;
}

float DavisAnemometer::getRollingWindSpeed() {
    float T = SamplePeriod/1000;
    return (float)countRotations(SamplePeriod) * (1.955196/T);
}

// Count the ticks that have occoured in the last n milliseconds.
// Used for rolling averages method.
int DavisAnemometer::countRotations(unsigned long int age) {
    int tickCount = 0;
    for (int i = 0; i < RotationsBufferSize; i++) {
        if ((millis() - RotationsBuffer[i]) <= age && RotationsBuffer[i] != 0) {
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
    serial.println("");
}
