#include <SerialRs485.h>

Auto485 bus(DE_PIN, DE_PIN, Serial);

void Rs485::begin(long baudrate) {
    bus.begin(baudrate);
}

void Rs485::sendPacket(void) {

}
