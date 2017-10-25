#include <Arduino.h>
#include <davis.h>
#include <timer.h>
#include <flash.h>

const int WindVanePin = A0;                 // The pin the wind vane sensor is connected to
const int WindVaneOffset  = 0;              // define the anemometer offset from magnetic north
const int WindSpeedPin = 2;                 // The pin location of the anemometer sensor
const unsigned long SamplePeriod = 3000;    // How often the windspeed is averaged over

const unsigned long ReadingPeriod = 1000;   // The interval between readings

DavisAnemometer davis(SamplePeriod, WindVanePin, WindSpeedPin, WindVaneOffset);
Timer t;
Flash f;

void TimerAction() {
    davis.print(Serial);
    f.ndFlash(50, 1);
}

void setup() {
    Serial.begin(57600);
    Serial.println("Welcome to Davis Anemometer");
    Serial.println("Reading initial sample.....");
    t.setTimer(ReadingPeriod, TimerAction);
    t.setDelay(SamplePeriod + ReadingPeriod);
    f.setPin(13);
}

void loop() {
    davis.update();
    t.update();
    f.update();
}
