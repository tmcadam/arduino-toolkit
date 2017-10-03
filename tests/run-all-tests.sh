#!/bin/bash
set -e

# A simple program to give a bit more output when debugging tests
g++ -g -std=gnu++11 -o  tests/TestSpike -I tests/mocks/ -I lib/ tests/TestSpike.cpp tests/mocks/Arduino.cpp
chmod +x tests/TestSpike
./tests/TestSpike
rm tests/TestSpike

# Run Flash tests
g++ -g -std=gnu++11 -o  tests/testFlash -I tests/mocks/ tests/TestFlash.cpp src/flash.cpp tests/mocks/Arduino.cpp -lcppunit
chmod +x tests/testFlash
./tests/testFlash
rm tests/testFlash

# Run Packet tests
g++ -g -std=gnu++11 -o  tests/testPacket -I tests/mocks/ -I lib/  lib/FastCRC/FastCRCsw.cpp tests/TestSerialPacket.cpp src/SerialComms/SerialPacket.cpp tests/mocks/Arduino.cpp -lcppunit
chmod +x tests/testPacket
./tests/testPacket
rm tests/testPacket
