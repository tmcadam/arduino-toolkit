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
    byte bVal[sizeof(long)];
};

union IntByte {
    unsigned int iVal;
    byte bVal[sizeof(int)];
};

typedef struct {
  IntByte direction;
  FloatByte speed;
} WindData;

typedef struct {
  byte pktType;
  byte dataType;
  byte dataLen;
  byte payload[32];
} Packet;

//Message outMessage;
extern Packet inPacket;
extern Packet outPacket;
//Global signals
extern boolean newMessageSignal;
extern boolean badMessageSignal;

enum pktType { DATA, COMMAND, CONFIRMATION};
enum dataType { ALL, HUMIDITY, PRESSURE, TEMPERATURE, WIND_SPEED, WIND_DIRECTION, WIND_ALL };

void busStart(long=BAUDRATE);
void sendDebug(char*);

void receivePacket();
void parsePacket();

void buildPacket(byte, byte, byte, byte*);
void sendPacket();

void bldWindPacket(WindData&);
void prsWindPacket(WindData&);

#endif
