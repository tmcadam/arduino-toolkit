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
//#pragma once
#ifndef MOCK_ARDUINO
#define MOCK_ARDUINO

//#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
//#define lowByte(w) ((unsigned char) ((w) & 0xff))
//#define highByte(w) ((unsigned char) ((w) >> 8))

//typedef unsigned short int word;
#include <stdio.h> // gives size_t
#include <string.h> // gives memset
#include <vector>
#include <map>
#include <chrono>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

//overrides PROGMEM to just use RAM
#define PROGMEM
inline uint8_t pgm_read_byte(const uint8_t * _address_short) {return *_address_short;};
inline uint16_t pgm_read_word(const uint16_t * _address_short) {return *_address_short;};
inline uint32_t pgm_read_dword(const uint32_t * _address_short) {return *_address_short;};

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define CHANGE 1
#define FALLING 2
#define RISING 3

typedef void (*GeneralCallbackFunction) ();

void pinMode(uint8_t, uint8_t);
void digitalWrite(int, int);
int digitalRead(int);
unsigned long millis();
void delay(unsigned long ms);
int analogRead(int);
void attachInterrupt(int, GeneralCallbackFunction, int);
int digitalPinToInterrupt(int);

// WMath.cpp
long map(long, long, long, long, long);
long random(long);
long random(long, long);

#include "fake_serial.h"

//---------------------Non Arduino Helpers------------------------------------//


struct DigitalWriteLog
{
    int pinNumber;
    unsigned long time;
    int pinValue;
};
extern std::vector<DigitalWriteLog> DigitalWriteLogs;

extern std::map<int, int> AnalogPinVals;
void setAnalogPinVal(int, int);

uint8_t getPinMode(uint8_t);
extern std::map<uint8_t, uint8_t> PinModes;

void initialize_mock_arduino();
void setCurrMillis(unsigned long);
unsigned long sysMillis();

#endif
