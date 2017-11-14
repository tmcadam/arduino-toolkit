#!/bin/bash
set -e

#A simple program to give a bit more output when debugging tests
g++-5 -g -std=gnu++11 -o  tests/TestSpike -I tests/mocks/ -I lib/ lib/FastCRC/FastCRCsw.cpp tests/TestSpike.cpp tests/mocks/Arduino.cpp src/SerialComms/SerialPacket.cpp
chmod +x tests/TestSpike
./tests/TestSpike
rm tests/TestSpike
