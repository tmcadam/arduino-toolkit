#ifndef SER_PAC_H
#define SER_PAC_H
#include <Arduino.h>

const byte PACKET_SIZE = 32;
const byte DELIM = '\0';
enum pktType { DATA, COMMAND, ACKNOWLEDGEMENT };

// Some unions to handle conversions to and from byte arrays
union FloatByte {
    float fVal = 0;
    byte bVal[sizeof(float)];
};
union LongByte {
    unsigned long int lVal = 0;
    byte bVal[sizeof(long)];
};
union IntByte {
    unsigned int iVal = 0;
    byte bVal[sizeof(int)];
};

class Packet {
    private:
        // properties
        byte pktType;
        byte dataType;
        byte dataLen;
        bool goodPacket;
        byte payload[PACKET_SIZE];
        byte tmpBuffer[PACKET_SIZE + 8];
        // methods
        unsigned int bytesToCrc(byte*, byte);
        void clearArray(byte*, byte);
        void addHeaderBytes(byte*, byte&);
        void addPayloadBytes(byte*, byte&);
        void addCrcBytes(byte*, byte&);
        void addCobsConversion(byte*, byte&);
        void addDelimiterBytes(byte*, byte&);
        void removeCobsConversion(byte*, byte&, byte*);
        bool crcMatch(byte&);
        void parseHeader(void);
        void parsePayload(void);

    public:
        // Constructors;
        Packet() { reset(); };
        void reset(void);
        void set(byte, byte, byte);
        void set(byte, byte, byte, byte*);

        // Getters
        byte getPktType() { return pktType; }
        byte getDataType() { return dataType; }
        byte getDataLen() { return dataLen; }
        bool getGoodPacket() { return goodPacket; }

        // Helpers for managing payload
        void getPayload(byte*);
        void getPayloadVal(byte*, byte&, byte); // needs a test
        void putPayloadVal(byte*, byte&, byte);

        // Creating.parsing byte arrays to send
        void setOutBuffer(byte*, byte&);
        void parse(byte*, byte&);
};

#endif
