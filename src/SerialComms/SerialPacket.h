#ifndef SER_PAC_H
#define SER_PAC_H
#include <Arduino.h>

const byte PACKET_SIZE = 32;
const byte DELIM = '\0';
enum pktType { DATA, COMMAND, ACKNOWLEDGEMENT };

// Some unions to handle conversions to and from byte arrays
union FloatByte {
    float fVal;
    byte bVal[sizeof(float)];
};
union LongByte {
    unsigned long int lVal;
    byte bVal[sizeof(long)];
};
union IntByte {
    unsigned int iVal;
    byte bVal[sizeof(int)];
};

class Packet {
    private:
        IntByte ib;
        bool goodPacket;
        unsigned long crc;
        byte tmpBuffer[PACKET_SIZE + 8];
        unsigned int bytesToCrc(byte*, byte);

    public:
        // ----- Packet Contents ------ //
        byte pktType;
        byte dataType;
        byte dataLen;
        byte payload[PACKET_SIZE];
        //void build(byte, byte, byte, byte*);
        void set(byte, byte, byte);
        void set(byte, byte, byte, byte*);
        void setOutBuffer(byte*, byte&);
        void parse(byte*, byte&);
        // some getters
        byte getPktType() { return pktType; }
        byte getDataType() { return dataType; }
        bool getGoodPacket() { return goodPacket; }
        void getPayload(byte*);
        // Helpers for packet payload building
        void getPayloadVal(byte*, byte, byte);
        static void putVal(byte*, byte*, byte, byte);
};

#endif
