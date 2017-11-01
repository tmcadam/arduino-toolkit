#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#define protected public
#define private   public
#include "../src/SerialComms/SerialRs485.h"
#undef protected
#undef private

using namespace std;
Rs485 rs485;

TEST_CASE ( "Test 1 is 1" ) {
    rs485.begin(57600);
    CHECK( 1 == 1 );
}
