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
    DavisAnemometer davis(3000, 7, 4, 7);
    setCurrMillis(3001); // Get past the weirdness during the first run
    setAnalogPinVal(7, 512);
    while(millis() <= 6100) {
        if (millis() % 500 == 0) {
            isrRotation(); //replicate an interrupt every 500ms
        }
        davis.update();
    }
    CHECK ( abs(float(3.91039) - davis.getRollingWindSpeed()) < 0.0001 );
    CHECK ( abs(float(3.91039) - davis.getSimpleWindSpeed()) < 0.0001 );
    CHECK ( 187 == davis.getDirection() );
}

//--------------------------Unit Tests----------------------------------------//

TEST_CASE ( "test getRollingWindSpeed returns expected speed" ) {
    DavisAnemometer davis(3000, 7, 4, 0);
    davis.updateRotationsBuffer(1000);
    davis.updateRotationsBuffer(2000);
    davis.updateRotationsBuffer(3000);
    davis.updateRotationsBuffer(4000);
    setCurrMillis(4010);
    CHECK ( abs(float(1.955196) - davis.getRollingWindSpeed()) < 0.0001 );
}

TEST_CASE ( "test countRotations returns the correct number of rotatins from RotationsBuffer" ) {
    DavisAnemometer davis(3000, 7, 4, 0);
    davis.updateRotationsBuffer(1000);
    davis.updateRotationsBuffer(2000);
    davis.updateRotationsBuffer(3000);
    setCurrMillis(3010);
    CHECK ( 3 == davis.countRotations());
    setCurrMillis(4010);
    davis.updateRotationsBuffer(4000);
    CHECK ( 3 == davis.countRotations());
}

TEST_CASE( "test updateSimpleWindSpeed" ) {
    DavisAnemometer davis(3000, 7, 4, 0);
    davis.LastSample = 0;
    SECTION ( "test updateSimpleWindSpeed updates if sample period exceeded" ) {
        davis.Rotations = 30;
        setCurrMillis(3000);
        davis.updateSimpleWindSpeed();
        CHECK ( abs(float(19.55196) - davis.SimpleWindSpeed) < 0.0001 );
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
    davis.updateRotationsBuffer(1000);
    davis.updateRotationsBuffer(1020);
    davis.updateRotationsBuffer(1040);
    CHECK ( davis.RotationsBuffer[0] == 1040 );
    CHECK ( davis.RotationsBuffer[1] == 1020 );
    CHECK ( davis.RotationsBuffer[2] == 1000 );
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