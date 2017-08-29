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
    void ndFlash(unsigned long, int);
    void dFlash(unsigned long, int);
    void ndWatcher();
};

#endif
