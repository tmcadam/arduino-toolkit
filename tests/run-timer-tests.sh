#!/bin/bash
set -e

# Run Catch tests
g++ -g -std=gnu++11 -o  tests/testTimer -I tests/mocks -I src/ tests/TestTimer.cpp src/timer.cpp tests/mocks/Arduino.cpp
chmod +x tests/testTimer
echo "Timer Tests"
./tests/testTimer
rm tests/testTimer
