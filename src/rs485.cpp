#include "rs485.h"
#define DE_PIN 3

// Hand over serial to the Auto485 (handles mode switching)
#if defined(__AVR_ATmega2560__)
  Auto485 bus(DE_PIN, DE_PIN, Serial1);
#else
  Auto485 bus(DE_PIN, DE_PIN, Serial);
#endif
//#define bus Serial //useful for debugging

// A CRC class
FastCRC16 CRC16;

// Byte arrays to send receive packets
byte inBuf[37];             //Payload + Header/CRC
byte inCobsBuf[38];         //Payload + Header/CRC + byte stuffing
byte outBuf[40];            //Payload + Header/CRC + byte stuffing + delims
byte outCobsBuf[38];        //Payload + Header/CRC + byte stuffing
// Better to have this defined once rather than constantly
// redifining with different sizes inside loop.

// complex types to store packets
Packet inPacket;
Packet outPacket;

//Global signals
boolean newMessageSignal = false;
boolean badMessageSignal = false;
boolean newOutMessageSignal = false;
//Local signals
boolean recvInProgress = false;
boolean sendInProgress = false;


LongByte lb;
FloatByte fb;
unsigned long int CRC;

void busStart(long busSpeed) {
    bus.begin(busSpeed);
}

void sendDebug(char* inChar){
    bus.println(inChar);
    //bus.flush();
}

void putVal(byte* payload, byte* bVal, byte startIdx, byte size) {
    for (byte i = 0; i < size; i++) {
        payload[startIdx] = bVal[i];
        startIdx++;
    }
}

void getVal(byte* bVal, byte* payload, byte startIdx, byte size) {
    for (byte i = 0; i < size; i++) {
        bVal[i] = payload[startIdx];
        startIdx++;
    }
}

void bldWindPacket(WindData &wd) {
    memset(outPacket.payload, 0, sizeof(outPacket.payload));
    outPacket.pktType = pktType::DATA;
    outPacket.dataType = dataType::WIND_ALL;
    outPacket.dataLen = sizeof(float) + sizeof(int);
    putVal(outPacket.payload, wd.speed.bVal, 0, sizeof(wd.speed.bVal));
    putVal(outPacket.payload, wd.direction.bVal, sizeof(wd.speed.bVal), sizeof(wd.direction.bVal));
}

void prsWindPacket(WindData &wd) {
    getVal(wd.speed.bVal, inPacket.payload, 0, sizeof(wd.speed.bVal));
    getVal(wd.direction.bVal, inPacket.payload, sizeof(wd.speed.bVal), sizeof(wd.direction.bVal));
}

IntByte ib;
void sendPacket() {
    // Make sure outBuf is empty
    memset(outBuf, 0, sizeof(outBuf));
    // Create an out buffer of the correct length
    byte packetSize = outPacket.dataLen + 5;    //add 5 bytes for header and CRC
    outBuf[0] = (byte)outPacket.pktType;        //packet type
    outBuf[1] = (byte)outPacket.dataType;       //data type
    outBuf[2] = (byte)outPacket.dataLen;        //data length
    for (byte i = 0; i < outPacket.dataLen; i++) {
        outBuf[i + 3] = outPacket.payload[i];
    }
    ib.iVal = CRC16.ccitt(outBuf, packetSize - 2);
    outBuf[packetSize - 2] = ib.bVal[0];        //CRC - 2nd from last byte
    outBuf[packetSize - 1] = ib.bVal[1];        //CRC - last byte

    memset(outCobsBuf, 0, sizeof(outCobsBuf));
    COBS::encode(outBuf, packetSize, outCobsBuf);
    packetSize+=3; // 1 extra COBS byte and 2 extra delimiter bytes
    outBuf[0] = DELIM;
    for (int i = 0; i < packetSize - 2; i++) {
        outBuf[i + 1] = outCobsBuf[i];
    }
    outBuf[packetSize - 1] = DELIM;
    bus.write(outBuf, packetSize);
    bus.flush();
}

bool parsePacket(byte size) {
    memset(inBuf, 0, sizeof(inBuf));
    memset(inPacket.payload, 0, sizeof(inPacket.payload));
    COBS::decode(inCobsBuf, size, inBuf);     //un-byte stuff the data
    ib.bVal[0] = inBuf[size - 3];                //parse the 2 CRC bits into a union
    ib.bVal[1] = inBuf[size - 2];                //parse the 2 CRC bits into a union
    CRC = CRC16.ccitt(inBuf, size - 3);          //calculate a 2 byte CRC
    if (CRC == ib.iVal ) {
      // read the data from a GOOD PACKET into a message object
      inPacket.pktType = (int)inBuf[0];
      inPacket.dataType = (int)inBuf[1];
      inPacket.dataLen = (int)inBuf[2];
      for (byte i = 0; i < inPacket.dataLen; i++) {
         inPacket.payload[i] = inBuf[i + 3]; //read the payload from packet
      }
      return true;
    }
    return false; //return false if the CRC vals don't match, BAD PACKET
}

int idx; //counter for message receive
byte rb; //byte for message receive
void receivePacket() {
    while (bus.available() > 0 && !newMessageSignal) {
        rb = bus.read();
        if (recvInProgress == true) {
            if (rb != DELIM) {
              inCobsBuf[idx] = rb;
              idx++;
            }
            else if ( rb == DELIM && idx > 0 ) {
                recvInProgress = false;
                if (parsePacket(idx)) {
                  newMessageSignal = true; //once this has been handled set to false
                } else {
                  badMessageSignal = true; //once this has been handled set to false
                }
                sei();
            }
        }
        else if (rb == DELIM) { //a new packet
            recvInProgress = true;
            newMessageSignal = false;
            memset(inCobsBuf, 0, sizeof(inCobsBuf));
            idx = 0;
        }
    }
}
