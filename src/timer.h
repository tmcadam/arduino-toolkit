#ifndef Timer_H
#define Timer_H
#include <Arduino.h>

typedef void (*GeneralCallbackFunction) ();

class Timer {
  private:
    unsigned long period;
    unsigned long lastAction;
    unsigned long offset;
    unsigned long delay;
    bool enabled;
    GeneralCallbackFunction action;

  public:
    void setPeriod(unsigned long, unsigned long _offset=0);
    void setAction(GeneralCallbackFunction);
    void setTimer(unsigned long, GeneralCallbackFunction, unsigned long _offset=0);
    void stop();
    void start();
    void update();
    void setDelay(unsigned long);
};

#endif
