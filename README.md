# Arduino Toolkit [![Build Status](https://travis-ci.org/tmcadam/arduino-toolkit.svg?branch=master)](https://travis-ci.org/tmcadam/arduino-toolkit)

A bunch of handy tools (and Arduino mocks) to use in Arduino projects.

## Install dependencies

Run build to install dependencies

  * ```cd <arduino-toolkit folder>```
  * ```bash build.sh```
    * You might get errors when it tries to run the tests, but don't worry about them (unless you want to run the tests), it will have downloaded dependencies by that point.

## Usage

In PlatformIO you need to do some extra configuration to the platform.ini file.
  * add ```build_flags = -I <path to toolkit src folder>```
    * i.e. ```build_flags = -I \home\tools\arduino-toolkit\src```


  * add ```src_filter = +< <path to src files> >```
    * i.e.```src_filter = +<\home\tools\arduino-toolkit\src\timer.cpp>```


  * You should then be able to use ```#include <timer.h>``` in your Arduino sketch.

## Tests

I used this project to get started Arduino test driven developemnt (TDD). I have done some work on mocking a subset of the Arduino objects and methods (based on https://github.com/IronSavior/dsm2_tx), which may be useful to others (see ```tests/mocks```). These basically allow you to develop your code without an Arduino hardware present, using a regular C++ compliler and testing framework (Catch2 - https://github.com/catchorg/Catch2).

Once you get beyond trivial sketches(and over the hump of creating the major mocks) I think this is a far quicker and robust way to produce Arduino code. It separates hardware weirdness (bad components, wobbly wires on breadboards etc.) from code weirdness, which makes it a lot easier to remove all the weirdness from your system.

To run the tests on Ubuntu, make sure you have ```g++-5``` installed, then run the build script.

  * ```cd <arduino-toolkit folder>```
  * ```bash build.sh```

There are other scripts to run all tests and individual test suites if needed, see the ```tests``` folder.


## ToDo

   * I am sure there are better ways to organise this project so it can work smoothly with both Arduino IDE and PlatformIO.
   * More examples
