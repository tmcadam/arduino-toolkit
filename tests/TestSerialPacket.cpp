#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#define protected public
#define private   public
#include "../src/SerialComms/SerialPacket.h"
#undef protected
#undef private

using namespace std;

TEST_CASE ( "Test getPayloadVal returns correct value from payload" ) {
    Packet mTestObj;
    byte idx = 0;
    IntByte ib;
    FloatByte fb;
    ib.iVal = 3456;
    fb.fVal = 36.786;
    memcpy( mTestObj.payload, ib.bVal, 2);
    memcpy( mTestObj.payload + 2, fb.bVal, 4);

    IntByte getIb;
    mTestObj.getPayloadVal(getIb.bVal, idx, 2);

    CHECK( getIb.iVal == 3456 );
    CHECK ( idx == 2 );

    FloatByte getFb;
    mTestObj.getPayloadVal(getFb.bVal, idx, 4);

    CHECK( float(getFb.fVal) == float(36.786) );
    CHECK ( idx == 6 );
}

TEST_CASE ( "Test reset sets Packet back to initilised state" ) {
    Packet mTestObj;
    mTestObj.pktType = 1;
    mTestObj.dataType = 2;
    mTestObj.dataLen = 6;
    mTestObj.goodPacket = true;
    byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    memcpy(mTestObj.payload, bufferArray, PACKET_SIZE);
    memcpy(mTestObj.tmpBuffer, bufferArray, PACKET_SIZE);

    mTestObj.reset();

    byte expectedArray[PACKET_SIZE];
    memset(expectedArray, 0, PACKET_SIZE);
    CHECK( mTestObj.pktType == 0 );
    CHECK( mTestObj.dataType == 0 );
    CHECK( mTestObj.dataLen == 0 );
    CHECK( mTestObj.goodPacket == false );
    CHECK(0 == memcmp(mTestObj.payload, expectedArray, PACKET_SIZE));
    CHECK(0 == memcmp(mTestObj.tmpBuffer, expectedArray, PACKET_SIZE));
}

TEST_CASE ( "Test putVal adds correct value into the Packet payload" ) {
    Packet mTestObj;
    byte idx = 0;

    // Add a float to payload
    FloatByte fbSet;
    fbSet.fVal = 46.7654;

    mTestObj.putPayloadVal(fbSet.bVal, idx, sizeof(float));

    FloatByte fbGet;
    fbGet.bVal[0] = mTestObj.payload[0];
    fbGet.bVal[1] = mTestObj.payload[1];
    fbGet.bVal[2] = mTestObj.payload[2];
    fbGet.bVal[3] = mTestObj.payload[3];
    CHECK(sizeof(float) == idx);
    CHECK(float(46.7654) == fbGet.fVal);

    // Add a int to payload
    IntByte ibSet;
    ibSet.iVal = 56754;

    mTestObj.putPayloadVal(ibSet.bVal, idx, sizeof(int));

    IntByte ibGet;
    ibGet.bVal[0] = mTestObj.payload[4];
    ibGet.bVal[1] = mTestObj.payload[5];
    CHECK(sizeof(float) + sizeof(int) == idx);
    CHECK(ibGet.iVal == 56754);
}

TEST_CASE ( "Test parse returns goodPacket if a packet is valid" ) {


    SECTION ( "Test a valid packet returns true" ) {
        Packet mTestObj;
        byte inBuffer[PACKET_SIZE] = {0x01,0x0B,0x01,0x06,0xB4,0xAF,0x98,0x1A,0xD1,0xAB,0x7D,0xA9};
        byte inBuffersize = 12;
        mTestObj.parse(inBuffer, inBuffersize);
        CHECK(true == mTestObj.getGoodPacket());
    }

    SECTION ( "Test a valid packet with no payload returns true" ) {
        Packet mTestObj;
        byte inBuffer[PACKET_SIZE] = {0x01, 0x02, 0x01, 0x03, 0xad, 0xff};
        byte inBuffersize = 6;
        mTestObj.parse(inBuffer, inBuffersize);
        CHECK(true == mTestObj.getGoodPacket());
    }

    SECTION ( "Test bad packet with one byte corrupted returns false" ) {
        Packet mTestObj;
        // B4 -> C4
        byte inBuffer[PACKET_SIZE] = {0x01,0x0B,0x01,0x06,0xC4,0xAF,0x98,0x1A,0xD1,0xAB,0x7D,0xA9};
        byte inBuffersize = 12;
        mTestObj.parse(inBuffer, inBuffersize);
        CHECK(false == mTestObj.getGoodPacket());
    }

    SECTION ( "Test bad packet partially truncated" ) {
        Packet mTestObj;
        byte inBuffer[PACKET_SIZE] = {0x01,0x0B,0x01,0x06,0xC4,0xAF,0x98,0x1A};
        byte inBuffersize = 8;
        mTestObj.parse(inBuffer, inBuffersize);
        CHECK(false == mTestObj.getGoodPacket());
    }

    SECTION ( "Test bad packet heavily truncates" ) {
        Packet mTestObj;
        byte inBuffer[PACKET_SIZE] = {0x01,0x0B,0x01,0x98,0x1A,0x00};
        byte inBuffersize = 6;
        mTestObj.parse(inBuffer, inBuffersize);
        CHECK(false == mTestObj.getGoodPacket());
    }

    // Packets with less than 6 bytes are invalid and shouldn't be parsed
}

TEST_CASE ( "Test parseHeader extracts class properties from packet" ) {
    Packet mTestObj;

    byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    memcpy(mTestObj.tmpBuffer, bufferArray, PACKET_SIZE);

    mTestObj.parseHeader();

    CHECK(byte(1) == mTestObj.pktType);
    CHECK(byte(2) == mTestObj.dataType);
    CHECK(byte(6) == mTestObj.dataLen);
}

TEST_CASE ( "Test parsePayload sets payload property" ) {
    Packet mTestObj;
    byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    memcpy(mTestObj.tmpBuffer, bufferArray, PACKET_SIZE);
    mTestObj.dataLen = 6;

    mTestObj.parsePayload();

    byte expectedArray[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    CHECK(0 == memcmp(mTestObj.payload, expectedArray, 6));
}

TEST_CASE ( "Test crcMatch returns correctly for valid and invalid packets" ) {
    Packet mTestObj;
    bool result;
    byte bufferSize;

    SECTION ( "Test returns true for a validpacket" ){
        byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
        memcpy(mTestObj.tmpBuffer, bufferArray, PACKET_SIZE);
        bufferSize = 11;
        result = mTestObj.crcMatch(bufferSize);
        CHECK(result == true);
    }

    SECTION ( "Test returns false for corrupted packet" ) {
        byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0x1a, 0xbf, 0x98, 0xd1, 0xab, 0x2b, 0x8a};
        memcpy(mTestObj.tmpBuffer, bufferArray, PACKET_SIZE);
        bufferSize = 11;
        CHECK(mTestObj.crcMatch(bufferSize) == false);
    }

    SECTION ( "Test return true for valid packet with no data" ) {
        byte bufferArray[PACKET_SIZE] = {0x01,0x02,0x00,0xce,0x9d};
        memcpy(mTestObj.tmpBuffer, bufferArray, PACKET_SIZE);
        bufferSize = 5;
        CHECK(mTestObj.crcMatch(bufferSize) == true);
    }
}

TEST_CASE ( "Test removeCobsConversion restores packet to pre-cobs state" ) {
    Packet mTestObj;
    byte bufferArray[PACKET_SIZE] = {0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    byte bufferSize = 12;
    byte tmpArray[PACKET_SIZE];

    mTestObj.removeCobsConversion(bufferArray, bufferSize, tmpArray);

    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};

    CHECK(byte(11) == bufferSize);
    CHECK(0 == memcmp(tmpArray, expectedArray, 11));
}

TEST_CASE ( "Test getPayload copies payload to array passed to function" ) {
    Packet mTestObj;
    mTestObj.dataLen = byte(6);
    byte setPayload[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    memcpy(mTestObj.payload, setPayload, 6);
    byte getPayload[PACKET_SIZE];

    mTestObj.getPayload(getPayload);

    CHECK(0 == memcmp(getPayload, setPayload, 6));
}

TEST_CASE ( "Test 'set' sets correct properties on Packet" ) {
    Packet mTestObj;
    byte pktType = 0;
    byte dataType = 1;
    byte dataLen = 6;

    SECTION ( "Test 'set' without payload provided" ) {
        mTestObj.set(pktType, dataType, dataLen);

        CHECK(byte(0) == mTestObj.pktType);
        CHECK(byte(1) == mTestObj.dataType);
        CHECK(byte(6) == mTestObj.dataLen);
        byte expectedArray[PACKET_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CHECK(0 == memcmp(mTestObj.payload, expectedArray, 6));
    }

    SECTION ( "Test 'set' with payload provided" ) {
        byte payload[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};

        mTestObj.set(pktType, dataType, dataLen, payload);

        CHECK(byte(0) == mTestObj.pktType);
        CHECK(byte(1) == mTestObj.dataType);
        CHECK(byte(6) == mTestObj.dataLen);
        CHECK(0 == memcmp(payload, mTestObj.payload, 9));
    }

}

TEST_CASE ( "Test addDelimterBytes sets correct bytes at the start and end of the packet byte array" ) {
    Packet mTestObj;
    byte testArray[PACKET_SIZE] = {0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    byte arraySize = 12;

    mTestObj.addDelimiterBytes(testArray, arraySize);

    byte expectedArray[PACKET_SIZE] = {DELIM, 0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a, DELIM};
    CHECK(byte(14) == arraySize);
    CHECK(0 == memcmp(testArray, expectedArray, 14));
}

TEST_CASE ( "Test addCobsConversion byte stuffs the packet byte array" ) {
    Packet mTestObj;
    byte testArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a, 0x00};
    byte arraySize = 11;

    mTestObj.addCobsConversion(testArray, arraySize);

    byte expectedArray[PACKET_SIZE] = {0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    CHECK(byte(12) == arraySize);
    CHECK(0 == memcmp(testArray, expectedArray, 12));
}

TEST_CASE ( "Test addCrcBytes add the two crc bytes to the end of the packet byte array" ) {
    Packet mTestObj;
    // https://www.lammertbies.nl/comm/info/crc-calculation.html
    // CRC-CCITT (0xFFFF)
    // hex 010206b4af981ad1ab
    // returns 0x8A2B
    byte testArray[PACKET_SIZE] = {0x01,0x02,0x06,0xb4,0xaf,0x98,0x1a,0xd1,0xab};
    byte arraySize = 9;

    mTestObj.addCrcBytes(testArray, arraySize);

    CHECK(byte(11) == arraySize);
    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    CHECK(0 == memcmp(testArray, expectedArray, 11));
}

TEST_CASE ( "Test addPayloadBytes adds the payload to the packet byte array" ) {
    Packet mTestObj;
    mTestObj.dataLen = byte(6);
    byte payload[mTestObj.dataLen] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    memcpy(mTestObj.payload, payload, 6);
    byte testArray[PACKET_SIZE] =  {0x01, 0x02, 0x06} ;
    byte arraySize = 3;

    mTestObj.addPayloadBytes(testArray, arraySize); //already tested

    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    CHECK(0 == memcmp(testArray, expectedArray, 9));
    CHECK(byte(9) == arraySize);
}

TEST_CASE ( "Test addHeaderBytes adds the header to the packet byte array" ) {
    Packet mTestObj;
    mTestObj.pktType = byte(1);
    mTestObj.dataType = byte(2);
    mTestObj.dataLen = byte(6);
    byte testArray[PACKET_SIZE];
    byte arraySize;

    mTestObj.addHeaderBytes(testArray, arraySize);

    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06};
    CHECK(0 == memcmp(testArray, expectedArray, 3));
}

TEST_CASE ( "Test clearBuffer sets all bytes in a byte array to 0x00" ) {
    Packet mTestObj;
    byte testArray[PACKET_SIZE]={0xb4,0xaf,0x98,0x1a,0xd1,0xab,0x21,0x1e,0xb9,0x34,0xcc,0x4d};

    mTestObj.clearArray(testArray, 12);

    byte expectedArray[PACKET_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    CHECK(0 == memcmp(testArray, expectedArray, 12));
}

TEST_CASE ( "Test bytesToCrc returns the expected integer for the byte array provided" ) {
    Packet mTestObj;
    // test that testBytesToCrc returns correct long value on array of bytes
    // https://www.lammertbies.nl/comm/info/crc-calculation.html
    // CRC-CCITT (0xFFFF)
    // hex string for online conversion b4af981ad1ab211eb934cc4d
    byte testArray[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x21, 0x1e, 0xb9, 0x34, 0xcc, 0x4d};

    uint16_t calculatedCrc = mTestObj.bytesToCrc(testArray, 12);

    uint16_t expectedCrc = 49615;
    CHECK(expectedCrc == calculatedCrc);
}

TEST_CASE ( "Test the setOutBufferProduces expected byte array" ) {
    Packet mTestObj;
    mTestObj.pktType = byte(0);
    mTestObj.dataType = byte(1);
    byte packetArray[PACKET_SIZE + 8];
    byte arraySize;

    SECTION ( "Test with payload" ) {
        mTestObj.dataLen = byte(6);
        byte payload[mTestObj.dataLen] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
        memcpy(mTestObj.payload, payload, 6);

        mTestObj.setOutBuffer(packetArray, arraySize);

        byte expectedPacket[PACKET_SIZE] = {0x00, 0x01, 0x0B, 0x01, 0x06, 0xB4, 0xAF, 0x98, 0x1A, 0xD1, 0xAB, 0x7D, 0xA9, 0x00};
        CHECK(byte(14) == arraySize);
        CHECK(0 == memcmp(expectedPacket, packetArray, 14));
    }

    SECTION ( "Test without payload" ) {
        mTestObj.dataLen = byte(0);

        mTestObj.setOutBuffer(packetArray, arraySize);

        byte expectedPacket[PACKET_SIZE] = {0x00, 0x01, 0x02, 0x01, 0x03, 0xad, 0xff, 0x00};
        CHECK(byte(8) == arraySize);
        CHECK(0 == memcmp(expectedPacket, packetArray, 8));
    }

}
