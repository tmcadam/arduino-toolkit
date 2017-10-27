#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>

#define protected public
#define private   public
#include "../src/SerialComms/SerialPacket.h"
#undef protected
#undef private

#include <Arduino.h>

using namespace std;

//-----------------------------------------------------------------------------

class TestPacket : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestPacket);

    CPPUNIT_TEST(testBytesToCrc);
    CPPUNIT_TEST(testParseInBuffer);
    CPPUNIT_TEST(testClearBuffer);
    CPPUNIT_TEST(testAddHeaderBytes);
    CPPUNIT_TEST(testAddPayloadBytes);
    CPPUNIT_TEST(testAddCrcBytes);
    CPPUNIT_TEST(testAddDelimiterBytes);
    CPPUNIT_TEST(testAddCobsConversion);
    CPPUNIT_TEST(testSetPacketOverload1);
    CPPUNIT_TEST(testSetPacketOverload2);
    CPPUNIT_TEST(testGetPayload);
    CPPUNIT_TEST(testRemoveCobsConversion);
    CPPUNIT_TEST(testCrcsMatch);
    CPPUNIT_TEST(testParseHeader);
    CPPUNIT_TEST(testParsePayload);
    CPPUNIT_TEST(testSetOutBufferWithPayload);
    CPPUNIT_TEST(testSetOutBufferWithoutPayload);

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testAddHeaderBytes(void);
    void testClearBuffer(void);
    void testBytesToCrc(void);
    void testSetOutBufferWithPayload(void);
    void testSetOutBufferWithoutPayload(void);
    void testParseInBuffer(void);
    void testAddPayloadBytes(void);
    void testAddCrcBytes(void);
    void testAddDelimiterBytes(void);
    void testAddCobsConversion(void);
    void testSetPacketOverload1(void);
    void testSetPacketOverload2(void);
    void testGetPayload(void);
    void testRemoveCobsConversion(void);
    void testCrcsMatch(void);
    void testParseHeader(void);
    void testParsePayload(void);
private:
    Packet *mTestObj;
};

//-----------------------------------------------------------------------------


void TestPacket::testParseInBuffer(void) {
    // Test good packet
    byte inBuffer[PACKET_SIZE] = {0x01,0x0B,0x01,0x06,0xB4,0xAF,0x98,0x1A,0xD1,0xAB,0x7D,0xA9};
    byte inBuffersize = 12;
    mTestObj->parse(inBuffer, inBuffersize);
    CPPUNIT_ASSERT_EQUAL(true, mTestObj->getGoodPacket());

    // Test good packet with no payload
    byte inBuffer4[PACKET_SIZE] = {0x01, 0x02, 0x01, 0x03, 0xad, 0xff};
    byte inBuffersize4 = 6;
    mTestObj->parse(inBuffer4, inBuffersize4);
    CPPUNIT_ASSERT_EQUAL(true, mTestObj->getGoodPacket());

    // Test bad packet if one byte corrupted B4 -> C4
    byte inBuffer1[PACKET_SIZE] = {0x01,0x0B,0x01,0x06,0xC4,0xAF,0x98,0x1A,0xD1,0xAB,0x7D,0xA9};
    byte inBuffersize1 = 12;
    mTestObj->parse(inBuffer1, inBuffersize1);
    CPPUNIT_ASSERT_EQUAL(false, mTestObj->getGoodPacket());

    // Test bad packet partially truncated
    byte inBuffer2[PACKET_SIZE] = {0x01,0x0B,0x01,0x06,0xC4,0xAF,0x98,0x1A};
    byte inBuffersize2 = 8;
    mTestObj->parse(inBuffer2, inBuffersize2);
    CPPUNIT_ASSERT_EQUAL(false, mTestObj->getGoodPacket());

    // Test bad packet heavily truncates
    byte inBuffer3[PACKET_SIZE] = {0x01,0x0B,0x01,0x98,0x1A,0x00};
    byte inBuffersize3 = 6;
    mTestObj->parse(inBuffer3, inBuffersize3);
    CPPUNIT_ASSERT_EQUAL(false, mTestObj->getGoodPacket());

    // Packets with less than 6 bytes are invalid and shouldn't be parsed
}

void TestPacket::testParseHeader(void) {
    // Test pass
    byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    memcpy(mTestObj->tmpBuffer, bufferArray, PACKET_SIZE);

    mTestObj->parseHeader();

    CPPUNIT_ASSERT_EQUAL(byte(1), mTestObj->pktType);
    CPPUNIT_ASSERT_EQUAL(byte(2), mTestObj->dataType);
    CPPUNIT_ASSERT_EQUAL(byte(6), mTestObj->dataLen);
}

void TestPacket::testParsePayload(void) {
    byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    memcpy(mTestObj->tmpBuffer, bufferArray, PACKET_SIZE);
    mTestObj->dataLen = 6;

    mTestObj->parsePayload();

    byte expectedArray[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    CPPUNIT_ASSERT_EQUAL(0, memcmp(mTestObj->payload, expectedArray, PACKET_SIZE));
}

void TestPacket::testCrcsMatch(void) {
    bool result;
    byte bufferSize;
    // Test pass
    byte bufferArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    memcpy(mTestObj->tmpBuffer, bufferArray, PACKET_SIZE);
    bufferSize = 11;
    result = mTestObj->crcMatch(bufferSize);
    CPPUNIT_ASSERT_EQUAL(result, true);
    // Test fail
    byte bufferArrayAltered[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0x1a, 0xbf, 0x98, 0xd1, 0xab, 0x2b, 0x8a};
    memcpy(mTestObj->tmpBuffer, bufferArrayAltered, PACKET_SIZE);
    bufferSize = 11;
    result = mTestObj->crcMatch(bufferSize);
    CPPUNIT_ASSERT_EQUAL(result, false);
    // Test pass with no data
    byte bufferArrayNoData[PACKET_SIZE] = {0x01,0x02,0x00,0xce,0x9d};
    memcpy(mTestObj->tmpBuffer, bufferArrayNoData, PACKET_SIZE);
    bufferSize = 5;
    result = mTestObj->crcMatch(bufferSize);
    CPPUNIT_ASSERT_EQUAL(result, true);
}

void TestPacket::testRemoveCobsConversion(void) {
    byte bufferArray[PACKET_SIZE] = {0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    byte bufferSize = 12;
    byte tmpArray[PACKET_SIZE];

    mTestObj->removeCobsConversion(bufferArray, bufferSize, tmpArray);

    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    CPPUNIT_ASSERT_EQUAL(byte(11), bufferSize);
    CPPUNIT_ASSERT_EQUAL(0, memcmp(tmpArray, expectedArray, 11));
}

void TestPacket::testGetPayload(void) {
    mTestObj->dataLen = byte(6);
    byte payload[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    memcpy(mTestObj->payload, payload, 6);

    mTestObj->getPayload(payload);

    CPPUNIT_ASSERT_EQUAL(0, memcmp(mTestObj->payload, payload, 6));
}

void TestPacket::testSetPacketOverload1(void) {
    byte pktType = 0;
    byte dataType = 1;
    byte dataLen = 6;

    mTestObj->set(pktType, dataType, dataLen);

    CPPUNIT_ASSERT_EQUAL(byte(0), mTestObj->pktType);
    CPPUNIT_ASSERT_EQUAL(byte(1), mTestObj->dataType);
    CPPUNIT_ASSERT_EQUAL(byte(6), mTestObj->dataLen);
    byte expectedArray[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CPPUNIT_ASSERT_EQUAL(0, memcmp(mTestObj->payload, expectedArray, 6));
}

void TestPacket::testSetPacketOverload2(void) {
    byte pktType = 0;
    byte dataType = 1;
    byte dataLen = 6;
    byte payload[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};

    mTestObj->set(pktType, dataType, dataLen, payload);

    CPPUNIT_ASSERT_EQUAL(byte(0), mTestObj->pktType);
    CPPUNIT_ASSERT_EQUAL(byte(1), mTestObj->dataType);
    CPPUNIT_ASSERT_EQUAL(byte(6), mTestObj->dataLen);
    CPPUNIT_ASSERT_EQUAL(0, memcmp(payload, mTestObj->payload, 6));
}

void TestPacket::testAddDelimiterBytes(void) {
    byte testArray[PACKET_SIZE] = {0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    byte arraySize = 12;

    mTestObj->addDelimiterBytes(testArray, arraySize);

    byte expectedArray[PACKET_SIZE] = {DELIM, 0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a, DELIM};
    CPPUNIT_ASSERT_EQUAL(byte(14), arraySize);
    CPPUNIT_ASSERT_EQUAL(0, memcmp(testArray, expectedArray, 14));
}

void TestPacket::testAddCobsConversion(void) {
    byte testArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a, 0x00};
    byte arraySize = 11;

    mTestObj->addCobsConversion(testArray, arraySize);

    byte expectedArray[PACKET_SIZE] = {0x0c, 0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    CPPUNIT_ASSERT_EQUAL(byte(12), arraySize);
    CPPUNIT_ASSERT_EQUAL(0, memcmp(testArray, expectedArray, 12));
}

void TestPacket::testAddCrcBytes(void) {
    // https://www.lammertbies.nl/comm/info/crc-calculation.html
    // CRC-CCITT (0xFFFF)
    // hex 010206b4af981ad1ab
    // returns 0x8A2B
    byte testArray[PACKET_SIZE] = {0x01,0x02,0x06,0xb4,0xaf,0x98,0x1a,0xd1,0xab};
    byte arraySize = 9;

    mTestObj->addCrcBytes(testArray, arraySize);

    CPPUNIT_ASSERT_EQUAL(byte(11), arraySize);
    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x2b, 0x8a};
    CPPUNIT_ASSERT_EQUAL(0, memcmp(testArray, expectedArray, 11));
}

void TestPacket::testAddPayloadBytes(void) {
    mTestObj->dataLen = byte(6);
    byte payload[mTestObj->dataLen] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    memcpy(mTestObj->payload, payload, 6);
    byte testArray[PACKET_SIZE] =  {0x01, 0x02, 0x06} ;
    byte arraySize = 3;

    mTestObj->addPayloadBytes(testArray, arraySize); //already tested

    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06, 0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    CPPUNIT_ASSERT_EQUAL(0, memcmp(testArray, expectedArray, 9));
    CPPUNIT_ASSERT_EQUAL(byte(9), arraySize);
}

void TestPacket::testAddHeaderBytes(void) {
    mTestObj->pktType = byte(1);
    mTestObj->dataType = byte(2);
    mTestObj->dataLen = byte(6);
    byte testArray[PACKET_SIZE];
    byte arraySize;

    mTestObj->addHeaderBytes(testArray, arraySize);

    byte expectedArray[PACKET_SIZE] = {0x01, 0x02, 0x06};
    CPPUNIT_ASSERT_EQUAL(0, memcmp(testArray, expectedArray, 3));
}

void TestPacket::testClearBuffer(void) {
    byte testArray[PACKET_SIZE]={0xb4,0xaf,0x98,0x1a,0xd1,0xab,0x21,0x1e,0xb9,0x34,0xcc,0x4d};

    mTestObj->clearArray(testArray, 12);

    byte expectedArray[PACKET_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    CPPUNIT_ASSERT_EQUAL(0, memcmp(testArray, expectedArray, 12));
}

void TestPacket::testBytesToCrc(void) {
    // test that testBytesToCrc returns correct long value on array of bytes
    // https://www.lammertbies.nl/comm/info/crc-calculation.html
    // CRC-CCITT (0xFFFF)
    // hex string for online conversion b4af981ad1ab211eb934cc4d
    byte testArray[PACKET_SIZE] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab, 0x21, 0x1e, 0xb9, 0x34, 0xcc, 0x4d};

    uint16_t calculatedCrc = mTestObj->bytesToCrc(testArray, 12);

    uint16_t expectedCrc = 49615;
    CPPUNIT_ASSERT_EQUAL(expectedCrc, calculatedCrc);
}

void TestPacket::testSetOutBufferWithPayload(void) {
    mTestObj->pktType = byte(0);
    mTestObj->dataType = byte(1);
    mTestObj->dataLen = byte(6);
    byte payload[mTestObj->dataLen] = {0xb4, 0xaf, 0x98, 0x1a, 0xd1, 0xab};
    memcpy(mTestObj->payload, payload, 6);
    byte packetArray[PACKET_SIZE + 8];
    byte arraySize;

    mTestObj->setOutBuffer(packetArray, arraySize);

    byte expectedPacket[PACKET_SIZE] = {0x00, 0x01, 0x0B, 0x01, 0x06, 0xB4, 0xAF, 0x98, 0x1A, 0xD1, 0xAB, 0x7D, 0xA9, 0x00};
    CPPUNIT_ASSERT_EQUAL(byte(14), arraySize);
    CPPUNIT_ASSERT_EQUAL(0, memcmp(expectedPacket, packetArray, 14));
}

void TestPacket::testSetOutBufferWithoutPayload(void) {
    mTestObj->pktType = byte(0);
    mTestObj->dataType = byte(1);
    mTestObj->dataLen = byte(0);
    byte packetArray[PACKET_SIZE + 8];
    byte arraySize;

    mTestObj->setOutBuffer(packetArray, arraySize);

    byte expectedPacket[PACKET_SIZE] = {0x00, 0x01, 0x02, 0x01, 0x03, 0xad, 0xff, 0x00};
    CPPUNIT_ASSERT_EQUAL(byte(8), arraySize);
    CPPUNIT_ASSERT_EQUAL(0, memcmp(expectedPacket, packetArray, 8));
}

void TestPacket::setUp(void) {
    mTestObj = new Packet();
    initialize_mock_arduino();
}

void TestPacket::tearDown(void) {
    delete mTestObj;
}

//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( TestPacket );

int main(int argc, char* argv[])
{
    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;

    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
