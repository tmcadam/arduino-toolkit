#ifndef Flash_H
#define Flash_H
#include <Arduino.h>

class Flash {
  private:
    int flashInProgress = 0;
    int startFlash = 0;
    int flashCounter;
    int pinNumber;
    unsigned long flashPeriod;
    int numFlashes;
    unsigned long flashStartTime = 0;

  public:
    void setPin(int);
    int getPin() { return pinNumber; };
    void ndFlash(unsigned long, int);
    void dFlash(unsigned long);
    void dFlashes(unsigned long, int);
    void ndWatcher();
    void update() { ndWatcher(); };
};

#endif
