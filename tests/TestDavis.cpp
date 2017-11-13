#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#define protected public
#define private   public
#include "../src/davis.h"
#undef protected
#undef private

#include <Arduino.h>

using namespace std;

//--------------------------Integration Tests---------------------------------//
TEST_CASE( "integration test the library" ) {
    SECTION ( "inetgeration test in normal mode" ) {
        DavisAnemometer davis(3000, 7, 4, 7);
        setCurrMillis(3001); // Get past the weirdness during the first run
        setAnalogPinVal(7, 512);
        while(millis() <= 6100) {
            if (millis() % 20 == 0) {
                isrRotation(); //replicate an interrupt every 500ms
                delay(1);
            }
            davis.update();
        }
        CHECK ( abs(float(50.292) - davis.getRollingWindSpeed()) < 0.0001 );
        CHECK ( abs(float(50.292) - davis.getSimpleWindSpeed()) < 0.0001 );
        CHECK ( 187 == davis.getDirection() );
    }
    SECTION  ( "inetgeration test in debug mode" ) {
        DavisAnemometer davis(3000, 7, 4, 7, true);
        float rolling = davis.getRollingWindSpeed();
        float simple = davis.getSimpleWindSpeed();
        float direction = davis.getDirection();
        CHECK ( (float(18.0) >= rolling && rolling >= float(12.0)) );
        CHECK ( (float(18.0) >= simple && simple >= float(12.0)) );
        CHECK ( (int(95) >= direction && direction >= int(85)) );
    }

}

//--------------------------Unit Tests----------------------------------------//

TEST_CASE ( "test getRollingWindSpeed returns expected speed" ) {
    DavisAnemometer davis(3000, 7, 4, 0);
    davis.updateRotationsBuffer(1000);
    davis.updateRotationsBuffer(2000);
    davis.updateRotationsBuffer(3000);
    davis.updateRotationsBuffer(4000);
    setCurrMillis(4010);
    CHECK ( abs(float(1.00584) - davis.getRollingWindSpeed()) < 0.0001 );
}

TEST_CASE ( "test countRotations returns the correct number of rotatins from RotationsBuffer" ) {
    DavisAnemometer davis(2000, 7, 4, 0);
    setCurrMillis(1);
    while(millis() <= 3100) {
        if (millis() % 20 == 0) {
            davis.updateRotationsBuffer(millis()); //replicate an interrupt every 500ms
            delay(1);
        }
    }
    CHECK (100 == davis.countRotations());
}

TEST_CASE( "test updateSimpleWindSpeed" ) {
    DavisAnemometer davis(3000, 7, 4, 0);
    davis.LastSample = 0;
    SECTION ( "test updateSimpleWindSpeed updates if sample period exceeded" ) {
        davis.Rotations = 30;
        setCurrMillis(3000);
        davis.updateSimpleWindSpeed();
        CHECK ( abs(float(10.0584) - davis.SimpleWindSpeed) < 0.0001 );
        CHECK ( 0 == davis.Rotations );
    }
    SECTION ( "test updateSimpleWindSpeed doesn't update if less than sample period" ) {
        davis.SimpleWindSpeed = 666666.666;
        davis.Rotations = 30;
        setCurrMillis(1000);
        davis.updateSimpleWindSpeed();
        CHECK ( abs(float(666666.666) - davis.SimpleWindSpeed) < 0.0001 );
    }
}

TEST_CASE ( "test UpdateRoatationsBuffer stores rotation times in reverse order" ) {
    DavisAnemometer davis(1000, 7, 4, 0);
    setCurrMillis(1);
    while(millis() <= 3100) {
        if (millis() % 20 == 0) {
            davis.updateRotationsBuffer(millis());
            delay(1); // make sure we don't repaeat in this millisecond
        }
    }
    CHECK ( davis.RotationsBuffer[0] == 3100 );
    CHECK ( davis.RotationsBuffer[1] == 3080 );
    CHECK ( davis.RotationsBuffer[2] == 3060 );
    CHECK ( davis.RotationsBuffer[147] == 160 );
    CHECK ( davis.RotationsBuffer[148] == 140 );
    CHECK ( davis.RotationsBuffer[149] == 120 );
}


TEST_CASE( "test Constructor" ) {
    DavisAnemometer davis(1000, 7, 4, 0);
    SECTION ( "test constructor sets windvane pin mode" ) {
        CHECK ( davis.WindVanePin == 7 );
        CHECK ( PinModes.count(7) == 1 );
        CHECK ( getPinMode(7) == INPUT );
    }
    SECTION ( "test constructor clears RotationBuffer" ) {
        unsigned long chkRotationsBuffer[ROTATION_BUFFER_SIZE];
        memset(chkRotationsBuffer, 0, ROTATION_BUFFER_SIZE);
        davis.updateRotationsBuffer(1000);
        davis.updateRotationsBuffer(1020);
        CHECK (0 != memcmp(chkRotationsBuffer, davis.RotationsBuffer, ROTATION_BUFFER_SIZE));
        DavisAnemometer davis(1000, 3, 4, 0);
        CHECK (0 == memcmp(chkRotationsBuffer, davis.RotationsBuffer, ROTATION_BUFFER_SIZE));
    }
    SECTION ( "test constructor sets rotations to 0" ) {
        davis.Rotations = 10;
        DavisAnemometer davis(1000, 7, 4, 0);
        CHECK ( 0 == davis.Rotations );
    }
}

TEST_CASE ( "test getDirection" ) {
    DavisAnemometer davis(1000, 3, 4, 0);
    SECTION ( "test getDirection returns expected values at 4 compass points" ) {
        setAnalogPinVal(3, 0);
        CHECK( davis.getDirection() == 0 );
        setAnalogPinVal(3, 256);
        CHECK( davis.getDirection() == 90 );
        setAnalogPinVal(3, 512);
        CHECK( davis.getDirection() == 180 );
        setAnalogPinVal(3, 768);
        CHECK( davis.getDirection() == 270 );
        setAnalogPinVal(3, 1023);
        CHECK( davis.getDirection() == 360 );
    }
    SECTION ( "test getDirection applies calibration if set" ) {
        davis.WindVaneOffset = 10;
        setAnalogPinVal(3, 512);
        CHECK( davis.getDirection() == 190 );
        davis.WindVaneOffset = -10;
        CHECK( davis.getDirection() == 170 );
    }
}

TEST_CASE ( "test handleInterrupt" ) {
    DavisAnemometer davis(1000, 3, 4, 0);
    davis.Rotations = 0;
    davis.ContactBounceTime = 0;
    SECTION ( "test handleInterupt adds 1 to rotations counter" ) {
        davis.handleInterrupt();
        CHECK( davis.Rotations == 1 );
    }
    SECTION ( "test handleInterupt doesn't add one to rotation counter if less than debounce limit" ) {
        davis.handleInterrupt();
        CHECK( davis.Rotations == 1 );
        davis.handleInterrupt();
        CHECK( davis.Rotations == 1 );
    }
    SECTION ( "test handleInterupt adds one to rotation counter if greater than debounce limit" ) {
        davis.handleInterrupt();
        CHECK( davis.Rotations == 1 );
        delay(20);
        davis.handleInterrupt();
        CHECK( davis.Rotations == 2 );
    }
}
