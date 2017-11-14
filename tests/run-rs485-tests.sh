#!/bin/bash
set -e

# Run Catch tests
echo "Rs485 Tests"
g++-5 -g -std=gnu++11 -o  tests/testRs485 -I lib/PacketSerial/src/Encoding -I tests/mocks/Auto485 -I tests/mocks -I lib/FastCRC -I src/SerialComms lib/FastCRC/FastCRCsw.cpp   tests/TestSerialRs485.cpp tests/mocks/fake_serial.cpp src/SerialComms/SerialPacket.cpp src/SerialComms/SerialRs485.cpp
chmod +x tests/testRs485
./tests/testRs485
rm tests/testRs485
