#include "SerialPacket.h"
FastCRC16 CRC16;

// Creates the final outbuffer of packet, writes to rs485 buffer, ready to send
void Packet::setOutBuffer(byte* Buffer, byte& BufferSize) {
    memset(Buffer, 0, PACKET_SIZE + 8); // Make sure outBuf is empty
    // Create an out buffer of the correct length
    BufferSize = dataLen + 5;    //add 5 bytes for header and CRC
    Buffer[0] = (byte)pktType;        //packet type
    Buffer[1] = (byte)dataType;       //data type
    Buffer[2] = (byte)dataLen;        //data length
    for (byte i = 0; i < dataLen; i++) {
        Buffer[i + 3] = payload[i];
    }
    ib.iVal = CRC16.ccitt(Buffer, BufferSize - 2);
    Buffer[BufferSize - 2] = ib.bVal[0];        //CRC - 2nd from last byte
    Buffer[BufferSize - 1] = ib.bVal[1];        //CRC - last byte
    memset(tmpBuffer, 0, sizeof(tmpBuffer));
    COBS::encode(Buffer, BufferSize, tmpBuffer);
    BufferSize+=3; // 1 extra COBS byte and 2 extra delimiter bytes
    Buffer[0] = DELIM;
    for (int i = 0; i < BufferSize - 2; i++) {
        Buffer[i + 1] = tmpBuffer[i];
    }
    Buffer[BufferSize - 1] = DELIM;
}

void Packet::set(byte _pktType, byte _dataType, byte _dataLen) {
    pktType = _pktType;
    dataType = _dataType;
    dataLen = _dataLen;
    memset(payload, 0, sizeof(payload));
}

void Packet::set(byte _pktType, byte _dataType, byte _dataLen, byte* _payload) {
    set(_pktType, _dataType, _dataLen);
    for (int i = 0; i < _dataLen; i++) {
        payload[i] = _payload[i];
    }
}

void Packet::getPayload(byte * _Buffer) {
    for (int i = 0; i < dataLen; i++) {
        _Buffer[i] = payload[i];
    }
}

void Packet::parse(byte* _Buffer, byte& _BufferSize) {
    goodPacket = false;                                     //set goodPacket false to start with
    memset(tmpBuffer, 0, sizeof(tmpBuffer));                //clean tmpBuffer
    memset(payload, 0, sizeof(payload));                    //clean payload
    COBS::decode(_Buffer, _BufferSize, tmpBuffer);          //un-byte stuff the data to tmpBuffer
    ib.bVal[0] = tmpBuffer[_BufferSize - 3];                //parse the 1st CRC byte into a union
    ib.bVal[1] = tmpBuffer[_BufferSize - 2];                //parse the 2nd CRC byte into a union
    crc = CRC16.ccitt(tmpBuffer, _BufferSize - 3);          //calculate a 2 byte CRC
    if (crc == ib.iVal ) {                                  //check in CRCs match
      pktType = (int)tmpBuffer[0];                          //parse packet type
      dataType = (int)tmpBuffer[1];                         //parse data type
      dataLen = (int)tmpBuffer[2];                          //parse data len
      for (byte i = 0; i < dataLen; i++) {
         payload[i] = tmpBuffer[i + 3];                     //read the payload from packet
      }
      goodPacket = true;                                    //set goodPacket true if CRC vals match
    }
}

// static method to help with building payload byte array (could these by non-static??)
void Packet::putVal(byte* _payload, byte* _bVal, byte _startIdx, byte _size) {
     for (byte i = 0; i < _size; i++) {
         _payload[_startIdx] = _bVal[i];
        _startIdx++;
    }
}

// method to help with parsing values from payload byte array
void Packet::getPayloadVal(byte* _bVal, byte _startIdx, byte _size) {
    for (byte i = 0; i < _size; i++) {
        _bVal[i] = payload[_startIdx];
        _startIdx++;
    }
}
