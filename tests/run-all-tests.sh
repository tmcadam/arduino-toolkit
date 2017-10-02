#!/bin/bash
set -e
g++ -g -std=gnu++11 -o  testFlash -Imocks/ TestFlash.cpp ../src/flash.cpp mocks/Arduino.cpp -lcppunit
chmod +x testFlash
./testFlash
rm testFlash
