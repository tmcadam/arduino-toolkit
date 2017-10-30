#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#define protected public
#define private   public
#include "../src/timer.h"
#undef protected
#undef private

#include <Arduino.h>

using namespace std;

void testCallBack(void);
std::vector<unsigned long> CallBackTimes;
Timer testTimer;

//--------------------------Test Cases----------------------------------------//

TEST_CASE ( "Integration: Test timer operates with expected behavior") {

    initialize_mock_arduino();
    testTimer.enabled = false;
    testTimer.delay = 0;
    testTimer.lastAction = 0;
    testTimer.offset = 0;
    testTimer.hasFirstAction = false;
    testTimer.hasFirstRun = false;

    SECTION ( "Timer stops triggering action if stopped" ) {
        CallBackTimes.clear();
        testTimer.setTimer(100, testCallBack);

        unsigned long startTime = millis();
        while ( (millis() - startTime) <= 500 ) {
            testTimer.update();
            if (CallBackTimes.size() == 2 ){
                testTimer.stop();
            }
        }

        CHECK (CallBackTimes.size() == 2);
    }

    SECTION ( "First timer action is triggered ASAP (without offset or delay)" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(10, testCallBack);
        delay(2);

        unsigned long startTime = millis();
        while ( (millis() - startTime) <= 50 ) {
            testTimer.update();
        }

        CHECK (CallBackTimes.front() - startTime == 0);
    }

    SECTION ( "Timer action is triggered ASAP + offset (no delay)" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(100, testCallBack, 50);
        delay(2);

        unsigned long startTime = millis();
        while ( (millis() - startTime) <= 60 ) {
            testTimer.update();
        }

        CHECK (CallBackTimes.front() -startTime == 50);
    }

    SECTION ( "100 ms period no offset" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(100, testCallBack);
        delay(2);

        unsigned long startTime = millis();
        while ( (millis() - startTime) <= 550 ) {
            testTimer.update();
        }

        CHECK (CallBackTimes.size() == 6);
        CHECK (CallBackTimes.front() - startTime == 0 );
        CHECK (CallBackTimes.back() - startTime == 500 );
    }

    SECTION ( "100ms and 50ms offset" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(100, testCallBack, 50);
        delay(2);

        unsigned long startTime = millis();
        while ( (millis() - startTime) <= 600 ) {
            testTimer.update();
        }

        CHECK (CallBackTimes.size() == 6);
        CHECK (CallBackTimes.front() - startTime == 50);
        CHECK (CallBackTimes.back() - startTime == 550);
    }

    SECTION ( "100 ms period, 120ms delay and no offset" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(100, testCallBack);
        testTimer.setDelay(120);
        delay(2);

        unsigned long startTime = millis();
        while ( (millis() - startTime) <= 650 ) {
            testTimer.update();
        }

        CHECK (CallBackTimes.size() == 6);
        CHECK (CallBackTimes.front() - startTime == 120 );
        CHECK (CallBackTimes.back() - startTime == 620 );
    }

    SECTION ( "100 ms period, 120ms delay and 50ms offset" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(100, testCallBack, 50);
        testTimer.setDelay(120);
        delay(2);

        unsigned long startTime = millis();
        while ( (millis() - startTime) <= 680 ) {
            testTimer.update();
        }

        CHECK (CallBackTimes.size() == 6);
        CHECK (CallBackTimes.front() - startTime == 170 );
        CHECK (CallBackTimes.back() - startTime == 670 );
    }

    SECTION ( "Timer can 'sleep' after it has started, by setting delay" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(100, testCallBack);
        testTimer.setDelay(120);
        delay(2);

        unsigned long startTime = millis();
        bool delayUpdated = false;
        while ( (millis() - startTime) <= 680 ) {
            testTimer.update();
            if ( CallBackTimes.size() == 3 && !delayUpdated ) {
                testTimer.setDelay(200);
                delayUpdated = true; // make sure we only do this once
            }
        }

        CHECK (CallBackTimes.size() == 5);
        CHECK (CallBackTimes[0] - startTime == 120 );
        CHECK (CallBackTimes[1] - startTime == 220 );
        CHECK (CallBackTimes[2] - startTime == 320 );
        CHECK (CallBackTimes[3] - startTime == 520 );
        CHECK (CallBackTimes[4] - startTime == 620 );
    }

    SECTION ( "Timer can 'sleep' after it has started, by setting stop and start" ) {
        CallBackTimes.clear();
        delay(3);
        testTimer.setTimer(100, testCallBack);
        delay(2);

        unsigned long startTime = millis();
        bool stopUpdated = false;
        bool startUpdated = false;
        while ( (millis() - startTime) <= 680 ) {
            testTimer.update();
            if ( (millis() - startTime) == 210 && !stopUpdated ) {
                testTimer.stop();
                stopUpdated = true; // make sure we only do this once
            } else if ( (millis() - startTime) == 420 && !startUpdated ) {
                testTimer.start();
                startUpdated = true; // make sure we only do this once
            }
        }

        CHECK (CallBackTimes.size() == 6);
        CHECK (CallBackTimes[0] - startTime == 0 );
        CHECK (CallBackTimes[1] - startTime == 100 );
        CHECK (CallBackTimes[2] - startTime == 200 );
        CHECK (CallBackTimes[3] - startTime == 420 );
        CHECK (CallBackTimes[4] - startTime == 520 );
        CHECK (CallBackTimes[5] - startTime == 620 );
    }
}

TEST_CASE ( "Unit: Test 'start' sets enabled" ) {

    initialize_mock_arduino();

    SECTION ("No delay") {
        testTimer.enabled = false;
        testTimer.hasFirstAction = true;
        testTimer.hasFirstRun = true;

        testTimer.start();

        CHECK ( testTimer.enabled == true );
        CHECK ( testTimer.hasFirstAction == false );
        CHECK ( testTimer.hasFirstRun == false );
    }
}

TEST_CASE ( "Unit: Test 'stop' sets enabled to false" ) {
    Timer testTimer;
    testTimer.enabled = true;

    testTimer.stop();

    CHECK(testTimer.enabled == false);
}

//------------------------------Helpers---------------------------------------//

void testCallBack(void) {
    CallBackTimes.push_back(millis());
}
