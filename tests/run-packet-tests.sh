#!/bin/bash
set -e

# Run Catch tests
echo "Packet Tests"
g++-5 -g -std=gnu++11 -o  tests/testPacket -I tests/mocks/ -I src/SerialComms -I lib/PacketSerial/src/Encoding -I lib/FastCRC lib/FastCRC/FastCRCsw.cpp tests/TestSerialPacket.cpp src/SerialComms/SerialPacket.cpp
chmod +x tests/testPacket
./tests/testPacket
rm tests/testPacket
