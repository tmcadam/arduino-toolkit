#!/bin/bash

echo "**> building arduino-toolkit"
# Go to the project directory
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${PROJECT_DIR}

# Install libs if they don't already exist
if [ ! -d "lib" ]; then
    echo "**> creating lib folder"
    mkdir lib
    echo "**> installing dependencies"
    git clone https://github.com/madleech/Auto485.git lib/Auto485
    git clone https://github.com/FrankBoesing/FastCRC.git lib/FastCRC
    git clone https://github.com/bakercp/PacketSerial.git lib/PacketSerial
fi

if [ ! -e "tests/catch.hpp" ]; then
    # Install the testing framework
    echo "**> installing Catch C++ testing framework"
    wget -P tests https://raw.githubusercontent.com/CatchOrg/Catch2/master/single_include/catch.hpp
fi

echo "**> starting tests"
bash "tests/run-all-tests.sh"
