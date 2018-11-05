// button.h
#ifndef Button_H
#define Button_H
#include <Arduino.h>

#ifdef __STM32F1__
  const WiringPinMode INTERNAL_RESISTOR = INPUT_PULLUP;
  const WiringPinMode EXTERNAL_RESISTOR = INPUT;
#else
  const int INTERNAL_RESISTOR = INPUT_PULLUP;
  const int EXTERNAL_RESISTOR = INPUT;
#endif

class Button {
  private:

    int pinNumber;
    int state = 0;
    unsigned long longHoldThreshold;
    unsigned long longHoldStartTime;
    int previousPosition = HIGH;
    int currentPosition = HIGH;
    unsigned long lastCheck;
    unsigned int checkDelay;
    void getPosition();

  public:
    #ifdef __STM32F1__
      void beginButton (int, unsigned long, unsigned int, WiringPinMode);
    #else
      void beginButton (int, unsigned long, unsigned int, int);
    #endif
    void setState(int);
    int checkButton();

};

struct PairResult {
  int pairStatus;
  int status1;
  int status2;
};

class ButtonPair {
  private:
    Button button1;
    Button button2;

  public:
    void setPair(Button button1, Button button2);
    PairResult checkPair();
  };

#endif
