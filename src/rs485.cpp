#include "rs485.h"

#define DE_PIN 3
// #define SERIAL_BUS //useful for debugging RS485 message processing

#ifdef SERIAL_BUS
  #define bus Serial
#else
  Auto485 bus(DE_PIN);
#endif

//String receivedString;
char receivedString[38];
char sendBuf[38];
Message inMessage;
Message outMessage;

boolean newMessageSignal = false;
boolean newOutMessageSignal = false;
boolean recvInProgress = false;
boolean sendInProgress = false;

int idx;

void busStart(long busSpeed) {
    bus.begin(busSpeed);
}

void sendDebug(char* inChar){
    bus.println(inChar);
    //bus.flush();
}

void parseMessage() {
    inMessage.msgType = receivedString[0] - '0';
    char buf[4];
    substring(receivedString, buf, 2, 4);
    inMessage.msgDataType = atoi(buf);
    substring(receivedString, inMessage.msgData, 6, strlen(receivedString) -1);
}

void buildMessage(int msgType, int msgDataType, char* msgData) {
    outMessage.msgType = msgType;
    outMessage.msgDataType = msgDataType;
    strcpy(outMessage.msgData, msgData);
}

void sendPacket() {

    sendBuf[0] = '\0';
    strcat(sendBuf, "<");
    char buf1[2];
    sprintf(buf1, "%01i", outMessage.msgType);
    strcat(sendBuf, buf1);
    strcat(sendBuf, ":");
    char buf2[4];
    sprintf(buf2, "%03X", outMessage.msgDataType);
    strcat(sendBuf, buf2);
    strcat(sendBuf, ":");
    strcat(sendBuf, outMessage.msgData);
    strcat(sendBuf, ">");
    bus.print(sendBuf);
    bus.flush();

}

void receivePacket() {
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (bus.available() > 0 && !newMessageSignal) {
        rc = bus.read();
        if (recvInProgress == true) {
            if (rc != endMarker) {
              receivedString[idx] = rc;
              idx++;
            }
            else {
                receivedString[idx] = '\0';
                recvInProgress = false;
                parseMessage();
                newMessageSignal = true;
            }
        }
        else if (rc == startMarker) {
            newMessageSignal = false;
            recvInProgress = true;
            receivedString[0] = '\0';
            idx = 0;
        }
    }
}
