#include <SerialRs485.h>

#if defined(__AVR_ATmega2560__)
  Auto485 bus(DE_PIN, DE_PIN, Serial1);
#else
  Auto485 bus(DE_PIN, DE_PIN, Serial);
#endif

void Rs485::begin(long baudrate) {
    bus.begin(baudrate);
}

void Rs485::sendPacket(void) {
    packet.setOutBuffer(packetArray, idx);
    bus.write(packetArray, idx);
}

void Rs485::read(GeneralCallbackFunction newPacketAction) {
    while (bus.available() > 0) {
        rb = bus.read();
        if (recvInProgress == true) {
            if (rb != DELIM) {
              // mid packet
              packetArray[idx] = rb;
              idx++;
            }
            else if ( rb == DELIM && idx > 0 ) {
                // end of packet
                recvInProgress = false;
                packet.parse(packetArray, idx);
                if (packet.getGoodPacket()) {
                    newPacketAction();
                };
            }
        }
        else if (rb == DELIM) {
            //a new packet
            recvInProgress = true;
            memset(packetArray, 0, sizeof(packetArray));
            idx = 0;
        }
    }
}
