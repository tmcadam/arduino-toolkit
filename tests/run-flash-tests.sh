#!/bin/bash
set -e

# Run Catch tests
g++ -g -std=gnu++11 -o  tests/testFlash -I tests/mocks tests/TestFlash.cpp src/flash.cpp tests/mocks/Arduino.cpp
chmod +x tests/testFlash
./tests/testFlash
rm tests/testFlash
