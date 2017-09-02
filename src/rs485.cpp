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
byte inBuf[32 + 5];             //Payload + Header/CRC
byte inCobsBuf[32 + 5 + 1];     //Payload + Header/CRC+ byte stuffing
byte outBuf[32 + 5 + 2 + 1];    //Payload + Header/CRC+ byte stuffing + delims + COBS
byte outCobsBuf[32 + 5 + 1];    //Payload + Header/CRC+ byte stuffing
// Better to have this defined once rather than constantly
// redifining with different sizes inside loop.

// Simple objects to store messages
FloatMessage inMessage;
FloatMessage outMessage;

//Global signals
boolean newMessageSignal = false;
boolean badMessageSignal = false;
boolean newOutMessageSignal = false;
//Local signals
boolean recvInProgress = false;
boolean sendInProgress = false;

int idx; //counter for message receive
byte rb; //byte for message receive
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

void buildMessage(int msgType, int dataType, float msgData) {
    outMessage.msgType = (byte)msgType;
    outMessage.dataType = (byte)dataType;
    outMessage.msgData.fVal = msgData;
}

void sendPacket() {
    // Make sure outBuf is empty
    memset(outBuf, 0, sizeof(outBuf));
    // Create an out buffer of the correct length
    int packetSize = outMessage.dataLen + 5; //add 5 bytes for header and CRC
    outBuf[0] = (byte)outMessage.msgType;    //packet type
    outBuf[1] = (byte)outMessage.dataType;       //data type
    outBuf[2] = (byte)outMessage.dataLen;        //data length
    for (byte i = 0; i < outMessage.dataLen; i++) {
        outBuf[i + 3] = outMessage.msgData.bVal[i];
    }
    lb.lVal = CRC16.ccitt(outBuf, packetSize - 2);
    outBuf[packetSize - 2] = lb.bVal[0]; //CRC - 2nd from last byte
    outBuf[packetSize - 1] = lb.bVal[1]; //CRC - last byte
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

bool parseMessage(byte size) {
    memset(inBuf, 0, sizeof(inBuf));
    COBS::decode(inCobsBuf, size, inBuf);     //un-byte stuff the data
    lb.bVal[0] = inBuf[size - 3];                //parse the 2 CRC bits into a union
    lb.bVal[1] = inBuf[size - 2];                //parse the 2 CRC bits into a union
    CRC = CRC16.ccitt(inBuf, size - 3);          //calculate a 2 byte CRC
    if (CRC != lb.lVal ) {
      return false;                             //return false if the CRC vals don't match, BAD PACKET
    } else {
      // read the data from a GOOD PACKET into a message object
      inMessage.msgType = (int)inBuf[0];
      inMessage.dataType = (int)inBuf[1];
      inMessage.dataLen = (int)inBuf[2];
      for (byte i = 0; i < inMessage.dataLen; i++) {
         inMessage.msgData.bVal[i] = inBuf[i + 3]; //read the payload from packet
      }
      return true;
    }
}

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
                if (parseMessage(idx)) {
                  newMessageSignal = true; //once this has been handled set to false
                } else {
                  badMessageSignal = true; //once this has been handled set to false
                }
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
