#ifndef SER_RS_H
#define SER_RS_H
#include <Arduino.h>
#include <SerialPacket.h>
#include <Auto485.h>

typedef void (*GeneralCallbackFunction) ();

#define DE_PIN 10

class Rs485 {
    private:
        byte rb;
        byte idx = 0;
        byte packetArray[PACKET_SIZE + 8];
        bool recvInProgress = false;

    public:
        Packet packet;
        void begin(long);
        void sendPacket(void);
        void read(GeneralCallbackFunction);
};

#endif
