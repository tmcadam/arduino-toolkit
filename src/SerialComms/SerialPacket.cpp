#include <FastCRC.h>
#include <COBS.h>
#include <SerialPacket.h>
FastCRC16 CRC16;


void Packet::reset(void) {
    pktType = 0x00;
    dataType = 0x00;
    dataLen = 0x00;
    goodPacket = false;
    clearArray(payload, sizeof(payload));
    clearArray(tmpBuffer, sizeof(tmpBuffer));
}

void Packet::putPayloadVal(byte* bVal, byte& idx, byte size) {
    memcpy(payload + idx, bVal, size);
    idx += size;
}

void Packet::getPayloadVal(byte* bVal, byte& idx, byte size) {
    memcpy(bVal, payload + idx, size);
    idx += size;
}

void Packet::removeCobsConversion(byte* bufferArray, byte& bufferSize, byte* tmpBuffer) {
    COBS::decode(bufferArray, bufferSize, tmpBuffer);
    bufferSize -= 1;
}

bool Packet::crcMatch(byte &bufferSize) {
    IntByte ib;
    ib.bVal[0] = tmpBuffer[bufferSize - 2];
    ib.bVal[1] = tmpBuffer[bufferSize - 1];
    bufferSize -= 2;
    return (bytesToCrc(tmpBuffer, bufferSize) == ib.iVal);
}

void Packet::parseHeader() {
    pktType = (int)tmpBuffer[0];
    dataType = (int)tmpBuffer[1];
    dataLen = (int)tmpBuffer[2];
}

void Packet::parsePayload() {
    memcpy(payload, tmpBuffer + 3, dataLen );
}

void Packet::parse(byte* bufferArray, byte& bufferSize) {
    reset();
    removeCobsConversion(bufferArray, bufferSize, tmpBuffer);
    if ( crcMatch(bufferSize) && bufferSize >= 3 ) {
        parseHeader();
        if ( bufferSize == (dataLen + 3) ) {
            parsePayload();
            goodPacket = true;
        }
    }
}

void Packet::getPayload(byte* payloadArray) {
    memcpy(payloadArray, payload, dataLen);
}

void Packet::set(byte _pktType, byte _dataType, byte _dataLen) {
    pktType = _pktType;
    dataType = _dataType;
    dataLen = _dataLen;
    memset(payload, 0, sizeof(payload));
}

void Packet::set(byte _pktType, byte _dataType, byte _dataLen, byte* _payload) {
    set(_pktType, _dataType, _dataLen);
    memcpy(payload, _payload, _dataLen);
}

void Packet::clearArray(byte* arrayToClear, byte sizeofArray) {
    memset(arrayToClear, 0, sizeofArray);
}

void Packet::addHeaderBytes(byte* packetArray, byte& arraySize) {
    packetArray[0] = (byte)pktType;
    packetArray[1] = (byte)dataType;
    packetArray[2] = (byte)dataLen;
    arraySize = 3;
}

void Packet::addPayloadBytes(byte* packetArray, byte& arraySize){
    for (byte i = 0; i < dataLen; i++) {
        packetArray[i + arraySize] = payload[i];
    }
    arraySize += dataLen;
}

unsigned int Packet::bytesToCrc(byte* packetArray, byte arraySize) {
    return CRC16.ccitt(packetArray, arraySize);
}

void Packet::addCrcBytes(byte* packetArray, byte& arraySize) {
    IntByte ib;
    ib.iVal = bytesToCrc(packetArray, arraySize);
    packetArray[arraySize] = ib.bVal[0];
    packetArray[arraySize + 1] = ib.bVal[1];
    arraySize += 2;
}

void Packet::addCobsConversion(byte* packetArray, byte& arraySize) {
    clearArray(tmpBuffer, sizeof(tmpBuffer));
    COBS::encode(packetArray, arraySize, tmpBuffer);
    arraySize += 1;
    memcpy(packetArray, tmpBuffer, arraySize);
}

void Packet::addDelimiterBytes(byte* packetArray, byte& arraySize) {
    memmove(packetArray + 1, packetArray, arraySize);
    packetArray[0] = DELIM;
    packetArray[arraySize + 1] = DELIM;
    arraySize += 2;
}

void Packet::setOutBuffer(byte* packetArray, byte& arraySize) {
    clearArray(packetArray, PACKET_SIZE + 8);
    addHeaderBytes(packetArray, arraySize);
    addPayloadBytes(packetArray, arraySize);
    addCrcBytes(packetArray, arraySize);
    addCobsConversion(packetArray, arraySize);
    addDelimiterBytes(packetArray, arraySize);
}
