#ifndef SER_RS_H
#define SER_RS_H
#include <Arduino.h>
#include <SerialPacket.h>
#include <Auto485.h>

#define DE_PIN 10

class Rs485 {
    private:
        Packet packet;

    public:
        void begin(long);
        void sendPacket(void);
};

#endif
