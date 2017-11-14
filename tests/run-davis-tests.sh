#!/bin/bash
set -e

# Run Catch tests
echo "Davis Tests"
g++-5 -g -std=gnu++11 -o  tests/testDavis -I tests/mocks tests/TestDavis.cpp src/davis.cpp tests/mocks/Arduino.cpp tests/mocks/fake_serial.cpp
chmod +x tests/testDavis
./tests/testDavis
rm tests/testDavis
