#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#define protected public
#define private   public
#include "../src/flash.h"
#undef protected
#undef private

#include <Arduino.h>

using namespace std;

uint16_t flashDuration(void);
int countFlashes(void);

//--------------------------Test Cases----------------------------------------//

TEST_CASE ( "testSetPin" ) {
    Flash mTestObj;
    mTestObj.setPin(10);
    CHECK(mTestObj.pinNumber  == 10);
    CHECK(digitalRead(10) == LOW );
}

TEST_CASE ( "testDFlash" ) {
    Flash mTestObj;
    unsigned long period = 20;
    unsigned long expectedDelay = period * 2;
    unsigned long startMillis = millis();
    DigitalWriteLogs.clear();
    mTestObj.setPin(10);
    mTestObj.dFlash(period);
    CHECK( expectedDelay == millis() - startMillis );
    CHECK( countFlashes() == 1 );
    CHECK( flashDuration() == period );
    CHECK( digitalRead(10) == LOW );
}

TEST_CASE ( "testDFlashes" ) {
    Flash mTestObj;
    int flashes = 3;
    unsigned long period = 10;
    unsigned long expectedDelay = ( flashes * period ) * 2;
    unsigned long startMillis = millis();
    DigitalWriteLogs.clear();
    mTestObj.setPin(10);
    mTestObj.dFlashes(period, flashes);
    CHECK( expectedDelay == millis() - startMillis );
    CHECK( countFlashes() == flashes );
    CHECK( flashDuration() == period  );
    CHECK( digitalRead(10) == LOW );
}

//-----------------------------Helper functions-------------------------------//

int countFlashes() {
    int c = 0;
    for(int i = 1; i != DigitalWriteLogs.size(); i++) {
        if ( DigitalWriteLogs[i].pinValue == HIGH && DigitalWriteLogs[i-1].pinValue == LOW )  {
            c++;
        }
    }
    return c;
}

uint16_t flashDuration() {
    unsigned long duration = DigitalWriteLogs[2].time - DigitalWriteLogs[1].time;
    return duration;
}
