#include "SerialController.h"

void SerialController::startRequestTransaction(byte _dataType) {
    // need to check if a transaction is in progress
    transStatus = masterState::COMMAND_SENT;
    transStartTime = millis();
    sendCommand(_dataType);
}

void SerialController::startResponseTransaction(byte _dataType) {
    transStatus = slaveState::COMMAND_RECEIVED;
    transStartTime = millis();
    //sendData(_dataType, _dataLen)
}

void SerialController::receivedPacket() {
    //Presuming master already
    if (inPacket.getGoodPacket()                          // check it's good
        && inPacket.getPktType() == pktType::DATA         // check it's data
        && transStatus == masterState::COMMAND_SENT) {   // check transaction is expecting it
            sendAcknowledgement(inPacket.getDataType());  // send an acknowledgement
            transStatus = masterState::MASTER_STAND_BY;  // complete the transaction
    }
    newPacketSignal = true;

    //requestCallback(inPacket);                            // call the callback to main application and pass the packet on to it
    // send all packets, good or bad

    // } else if (packet.getGoodPacket()                          // check it's good
    //         && packet.getPktType() == pktType::COMMAND         // check it's data
    //         && transStatus == slaveState::STAND_BY ) {
    //             startResponseTransaction(packet.getDataType());
    // }
    //do we only send good packets to main application?

}
