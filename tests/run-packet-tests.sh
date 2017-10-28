#!/bin/bash
set -e

# Run Catch tests
g++ -g -std=gnu++11 -o  tests/testPacket -I tests/mocks/ -I lib/  lib/FastCRC/FastCRCsw.cpp tests/TestSerialPacket.cpp src/SerialComms/SerialPacket.cpp
chmod +x tests/testPacket
./tests/testPacket
rm tests/testPacket
