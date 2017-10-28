#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#define protected public
#define private   public
#include "../src/timer.h"
#undef protected
#undef private

#include <Arduino.h>

using namespace std;

//--------------------------Test Cases----------------------------------------//

TEST_CASE ( "Hello world" ) {
    CHECK ("Helloworld!" == "Helloworld!");
}
