#include "SerialController.h"

// constructor, set a few initial values
SerialController::SerialController () {
    newPacketSignal = false;
    sendSignal = false;
    recvInProgress = false;
    sendInProgress = false;
    lastByteReceived = 0;
}
