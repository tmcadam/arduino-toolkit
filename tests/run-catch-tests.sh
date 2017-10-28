#!/bin/bash
set -e

# Run Catch tests
g++ -g -std=gnu++11 -o  tests/testCatch -I tests/mocks/ -I lib/  lib/FastCRC/FastCRCsw.cpp tests/TestSerialPacket.cpp src/SerialComms/SerialPacket.cpp tests/mocks/Arduino.cpp
chmod +x tests/testCatch
./tests/testCatch
rm tests/testCatch
