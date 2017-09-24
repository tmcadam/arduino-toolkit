#include "SerialController.h"
#include "../Auto485/Auto485.h"

// Hand over serial to the Auto485 (handles mode switching)
#if defined(__AVR_ATmega2560__)
  Auto485 bus(DE_PIN, DE_PIN, Serial1);
#else
  Auto485 bus(DE_PIN, DE_PIN, Serial);
#endif
//#define bus Serial //useful for debugging

// runs on main loop to send or receive messages, perform transaction actions
void SerialController::update() {
    receive();
    send();
}

// starts the bus at the correct baud rate
void SerialController::begin(byte _devType, long _busSpeed) {
    deviceRole = _devType;
    bus.begin(_busSpeed);
    if (deviceRole == devType::MASTER) {
        transStatus = masterState::MASTER_STAND_BY;
    } else if (deviceRole == devType::SLAVE) {
        transStatus = slaveState::SLAVE_STAND_BY;

    }
}

// set a signal that there is a packet ready to send
void SerialController::sendPacket() {
    sendSignal = true;
}

// build and send a data packet (with payload)
void SerialController::sendDataPacket(byte _dataType, byte _dataLen, byte * _payload) {
    outPacket.set(pktType::DATA, _dataType, _dataLen, _payload);
    outPacket.setOutBuffer(Buffer, BufferSize);
    sendPacket();
}

// send a simple packet (without payload)
void SerialController::sendSimplePacket(byte _pktType, byte _dataType) {
    outPacket.set(_pktType, _dataType, 0);
    outPacket.setOutBuffer(Buffer, BufferSize);
    sendPacket();
}

// create simple command packet and send it
void SerialController::sendCommand(byte _dataType) {
    sendSimplePacket(pktType::COMMAND, _dataType);
}

// create a simple acknowledgement packet and send it
void SerialController::sendAcknowledgement(byte _dataType) {
    sendSimplePacket(pktType::ACKNOWLEDGEMENT, _dataType);
}

// watch the sendSignal flag and sends packet
void SerialController::send() {
    if (sendSignal && !recvInProgress && !sendInProgress &&
        (millis() - lastByteReceived) > SEND_RECEIVE_GAP) {
            sendInProgress = true;
            sendSignal = false;
            bus.write(Buffer, BufferSize);
            bus.flush();
            sendInProgress = false;
    }
}

// watch the bus for new data arriving
void SerialController::receive() {
    while (bus.available() > 0) {
        rb = bus.read();
        lastByteReceived = millis();
        if (recvInProgress == true) {
            if (rb != DELIM) {
              Buffer[BufferIdx] = rb;
              BufferIdx++;
            }
            else if ( rb == DELIM && BufferIdx > 0 ) {
                recvInProgress = false;
                inPacket.parse(Buffer, BufferIdx);
                receivedPacket();
            }
        }
        else if (rb == DELIM) { //a new packet
            recvInProgress = true;
            memset(Buffer, 0, BufferSize);
            BufferIdx = 0;
        }
    }
}
