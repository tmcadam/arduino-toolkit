#!/bin/bash
set -e

# Run Catch tests
echo "Timer Tests"
g++-5 -g -std=gnu++11 -o  tests/testTimer -I tests/mocks tests/TestTimer.cpp timer.cpp tests/mocks/Arduino.cpp
chmod +x tests/testTimer
./tests/testTimer
rm tests/testTimer
