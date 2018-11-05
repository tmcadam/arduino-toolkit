#ifndef Timer_H
#define Timer_H
#include <Arduino.h>

typedef void (*GeneralCallbackFunction) ();

class Timer {
  private:
    unsigned long period;
    unsigned long lastAction;
    unsigned long offset;
    unsigned long delay = 0;
    bool enabled;
    bool hasFirstAction;
    bool hasFirstRun;
    GeneralCallbackFunction action;

  public:
    // setters
    void setTimer(unsigned long, GeneralCallbackFunction, unsigned long _offset=0);
    void setPeriod(unsigned long, unsigned long _offset=0);
    void setAction(GeneralCallbackFunction);
    void setDelay(unsigned long);
    // actions
    void stop();
    void start();
    void update();
};

#endif
