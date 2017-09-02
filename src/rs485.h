#ifndef Rs485_H
#define Rs485_H
#include <Arduino.h>
#include <Auto485/Auto485.h>
#include <FastCRC/FastCRC.h>
#include <PacketSerial/src/Encoding/COBS.h>
#include <strings.h>

//A reserved vyte that delimits messages
const byte DELIM = '\0';
const long BAUDRATE = 57600;

union FloatByte {
  float fVal;
  byte bVal[sizeof(float)];
};

union LongByte {
    unsigned long int lVal;
    byte bVal[2];
};

typedef struct {
  int msgType;
  int dataType;
  int dataLen = sizeof(float);
  FloatByte msgData;
} FloatMessage;

//Message outMessage;
extern FloatMessage inMessage;
//Global signals
extern boolean newMessageSignal;
extern boolean badMessageSignal;

enum { DATA, COMMAND, CONFIRMATION};
enum { ALL, HUMIDITY, PRESSURE, TEMPERATURE, WIND_SPEED, WIND_DIRECTION };

void receivePacket();
void parseMessage();
void buildMessage(int, int, float);
void sendPacket();
void busStart(long=BAUDRATE);
void sendDebug(char*);

#endif
