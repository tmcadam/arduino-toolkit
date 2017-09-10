#ifndef Timer_H
#define Timer_H
#include <Arduino.h>

typedef void (*GeneralCallbackFunction) ();

class Timer {
  private:
    unsigned long period;
    unsigned long lastAction;
    bool enabled;
    GeneralCallbackFunction action;

  public:
    void setPeriod(unsigned long);
    void setAction(GeneralCallbackFunction);
    void setTimer(unsigned long, GeneralCallbackFunction);
    void stop();
    void start();
    void update();
};

#endif
