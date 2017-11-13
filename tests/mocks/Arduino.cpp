/*
  DSM2_tx implements the serial communication protocol used for operating
  the RF modules that can be found in many DSM2-compatible transmitters.

  Copyrigt (C) 2012  Erik Elmore <erik@ironsavior.net>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>

std::vector<DigitalWriteLog> DigitalWriteLogs;
std::map<int, int> AnalogPinVals;
std::map<uint8_t, uint8_t> PinModes;

unsigned long t_start_millis;
unsigned long millis() {
    unsigned long t_now_millis = sysMillis();
    return t_now_millis - t_start_millis;
}

void delay(unsigned long ms) {
    unsigned long start = millis();
    while(millis() - start < ms){}
}

void digitalWrite(int pinNumber, int pinState) {
    DigitalWriteLog log = {pinNumber, millis(), pinState};
    DigitalWriteLogs.push_back(log);
}

void pinMode(uint8_t pinNumber, uint8_t pinMode) {
    PinModes[pinNumber] = pinMode;
    DigitalWriteLog log = {pinNumber, millis(), LOW};
    DigitalWriteLogs.push_back(log);
}

int digitalRead(int pinNumber) {
    for(int i=DigitalWriteLogs.size() -1; i >= 0; i--) {
        if (DigitalWriteLogs[i].pinNumber == pinNumber) {
            return DigitalWriteLogs[i].pinValue;
        }
    }
}

int analogRead(int pinNumber) {
    return AnalogPinVals[pinNumber];
}

void attachInterrupt(int pinNumber, GeneralCallbackFunction callBack, int state) {}

int digitalPinToInterrupt(int pinNumber) {
    return 0;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long random(long howbig)
{
  if (howbig == 0) {
    return 0;
  }
  return random() % howbig;
}

long random(long howsmall, long howbig)
{
  if (howsmall >= howbig) {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return random(diff) + howsmall;
}


//---------------------Non Arduino Helpers------------------------------------//

uint8_t getPinMode(uint8_t pinNumber) {
    return PinModes[pinNumber];
}

void setAnalogPinVal(int pinNumber, int value) {
    AnalogPinVals[pinNumber] = value;
}

void initialize_mock_arduino() {
    t_start_millis = sysMillis();
}

void setCurrMillis(unsigned long currMillis) {
    t_start_millis = sysMillis() - currMillis;
}

unsigned long sysMillis() {
    using namespace std::chrono;
    return system_clock::now().time_since_epoch() / milliseconds(1);
}
