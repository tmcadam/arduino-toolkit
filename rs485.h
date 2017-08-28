#ifndef Rs485_H
#define Rs485_H
#include <Arduino.h>
#include <lib/Auto485/Auto485.h>
#include <strings.h>

typedef struct {
  int msgType;
  int msgDataType;
  char msgData[33]; // 32 characters of data
} Message;

//Message outMessage;
extern Message inMessage;
extern boolean newMessageSignal;

enum { DATA, COMMAND, CONFIRMATION};
enum { ALL, HUMIDITY, PRESSURE, TEMPERATURE, WIND_SPEED, WIND_DIRECTION };

void receivePacket();
void parseMessage();
void buildMessage(int, int, char*);
void sendPacket();
void busStart(long);
void sendDebug(char*);

#endif
