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
    CPPUNIT_TEST(testHelloWorld);
    CPPUNIT_TEST(testBytesToCrc);
    CPPUNIT_TEST(testSetOutBuffer);
    CPPUNIT_TEST(testParseInBuffer);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testHelloWorld(void);
    void testBytesToCrc(void);
    void testSetOutBuffer(void);
    void testParseInBuffer(void);

private:
    int countFlashes(void);
    unsigned long flashDuration(void);
    byte Buffer[PACKET_SIZE + 8];
    byte Buffersize;
    Packet *mTestObj;
};

//-----------------------------------------------------------------------------

void TestPacket::testHelloWorld(void) {
    CPPUNIT_ASSERT_EQUAL("Helloworld", "Helloworld");
    CPPUNIT_ASSERT("Hello1world" != "Helloworld");
}

void TestPacket::testBytesToCrc(void) {
    uint16_t expectedCrc = 49615;
    // hex string for online conversion b4af981ad1ab211eb934cc4d
    byte testArray[12]={0xb4,0xaf,0x98,0x1a,0xd1,0xab,0x21,0x1e,0xb9,0x34,0xcc,0x4d};
    uint16_t calculatedCrc = mTestObj->bytesToCrc(testArray, 12);
    CPPUNIT_ASSERT_EQUAL(expectedCrc, calculatedCrc);
}

void TestPacket::testSetOutBuffer(void) {
    const byte DATA_TYPE = 1;
    byte data_len = 6;
    byte payload[6] = {0xb4,0xaf,0x98,0x1a,0xd1,0xab};
    byte expectedBuffer[14] = {0x00,0x01,0x0B,0x01,0x06,0xB4,0xAF,0x98,0x1A,0xD1,0xAB,0x7D,0xA9,0x00};
    byte expectedBuffersize = data_len + 8;

    mTestObj->set(DATA, DATA_TYPE, data_len, payload);
    mTestObj->setOutBuffer(Buffer, Buffersize);

    CPPUNIT_ASSERT_EQUAL(expectedBuffersize, Buffersize);
    CPPUNIT_ASSERT_EQUAL(0, memcmp(expectedBuffer, Buffer, expectedBuffersize));
}

void TestPacket::testParseInBuffer(void) {
    byte inBuffer[12] = {0x01,0x0B,0x01,0x06,0xB4,0xAF,0x98,0x1A,0xD1,0xAB,0x7D,0xA9};
    byte inBuffersize = 12;
    byte expectedPayload[6] = {0xb4,0xaf,0x98,0x1a,0xd1,0xab};
    byte expectedPayloadsize = 6;
    mTestObj->parse(inBuffer, inBuffersize);
    CPPUNIT_ASSERT_EQUAL(true, mTestObj->getGoodPacket());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(expectedPayload, mTestObj->payload, expectedPayloadsize));
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
