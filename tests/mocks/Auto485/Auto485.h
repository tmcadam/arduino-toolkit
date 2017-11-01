#ifndef MOCK_AUT0485
#define MOCK_AUT0485

#include <Arduino.h>

class Auto485 {
    public:
        Auto485() {};
        Auto485(int, int, FakeSerial&) {};
        void begin(long) {};
};

#endif
