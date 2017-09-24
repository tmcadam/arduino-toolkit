#ifndef SER_CONT_H
#define SER_CONT_H
#include "SerialPacket.h"

const unsigned long SEND_RECEIVE_GAP = 2;           //in milliseconds
const unsigned long TRANSACTION_TIMEOUT = 10;       //in milliseconds
const byte DE_PIN = 3;
const long BAUDRATE = 57600;
enum devType { MASTER, SLAVE };
enum slaveState { SLAVE_STAND_BY, COMMAND_RECEIVED, DATA_SENT };
enum masterState { MASTER_STAND_BY, COMMAND_SENT };

class SerialController {

    // ----- Controller ------//

    private:
        Packet outPacket;
        Packet inPacket;
        byte deviceRole;
        byte transStatus;
        unsigned long transStartTime;
        bool recvInProgress;
        bool sendInProgress;
        bool sendSignal;
        //bool badPacketSignal;
        unsigned long lastByteReceived;
        byte Buffer[PACKET_SIZE + 8];
        byte BufferSize;
        byte BufferIdx;
        byte rb;
        void send();
        void receive();
        void receivedPacket();
        void sendCommand(byte);
        void sendAcknowledgement(byte);
        void sendSimplePacket(byte, byte);
        void sendPacket();
        bool newPacketSignal;

    public:
        SerialController();
        void startRequestTransaction(byte);
        void startResponseTransaction(byte);
        void sendDataPacket(byte, byte, byte*);
        void begin(byte, long);
        bool getPacketSignal() { return newPacketSignal; }
        void clearPacketSignal() {newPacketSignal = false;}
        Packet& getInPacket() {return inPacket;}
        void update();
};
#endif
