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
#include "../src/flash.h"
#undef protected
#undef private
#include <Arduino.h>

using namespace std;

//-----------------------------------------------------------------------------

class TestFlash : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestFlash);
    CPPUNIT_TEST(testHelloWorld);
    CPPUNIT_TEST(testSetPin);
    CPPUNIT_TEST(testDFlash);
    CPPUNIT_TEST(testDFlashes);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testHelloWorld(void);
    void testSetPin(void);
    void testDFlash(void);
    void testDFlashes(void);

private:
    int countFlashes(void);
    unsigned long flashDuration(void);
    Flash *mTestObj;
};

//-----------------------------------------------------------------------------

void TestFlash::testHelloWorld(void) {
    CPPUNIT_ASSERT_EQUAL("Helloworld", "Helloworld");
    CPPUNIT_ASSERT("Hello1world" != "Helloworld");
}

void TestFlash::testSetPin(void) {
    mTestObj->setPin(10);
    CPPUNIT_ASSERT_EQUAL(mTestObj->pinNumber , 10);
    CPPUNIT_ASSERT(digitalRead(10) == LOW );
}

void TestFlash::testDFlash(void) {
    unsigned long period = 20;
    unsigned long expectedDelay = period * 2;
    unsigned long startMillis = millis();
    DigitalWriteLogs.clear();
    mTestObj->setPin(10);
    mTestObj->dFlash(period);
    CPPUNIT_ASSERT( expectedDelay == millis() - startMillis );
    CPPUNIT_ASSERT( countFlashes() == 1 );
    CPPUNIT_ASSERT( flashDuration() == period );
    CPPUNIT_ASSERT( digitalRead(10) == LOW );
}

void TestFlash::testDFlashes(void) {
    int flashes = 3;
    unsigned long period = 10;
    unsigned long expectedDelay = ( flashes * period ) * 2;
    unsigned long startMillis = millis();
    DigitalWriteLogs.clear();
    mTestObj->setPin(10);
    mTestObj->dFlashes(period, flashes);
    CPPUNIT_ASSERT( expectedDelay == millis() - startMillis );
    CPPUNIT_ASSERT( countFlashes() == flashes );
    CPPUNIT_ASSERT( flashDuration() == period  );
    CPPUNIT_ASSERT( digitalRead(10) == LOW );
}

int TestFlash::countFlashes() {
    int c = 0;
    for(int i = 1; i != DigitalWriteLogs.size(); i++) {
        if ( DigitalWriteLogs[i].pinValue == HIGH && DigitalWriteLogs[i-1].pinValue == LOW )  {
            c++;
        }
    }
    return c;
}

unsigned long TestFlash::flashDuration() {
    unsigned long duration = DigitalWriteLogs[2].time - DigitalWriteLogs[1].time;
    return duration;
}

void TestFlash::setUp(void) {
    mTestObj = new Flash();
    initialize_mock_arduino();
}

void TestFlash::tearDown(void) {
    delete mTestObj;
}

//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( TestFlash );

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
