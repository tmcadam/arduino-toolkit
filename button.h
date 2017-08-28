// button.h
#ifndef Button_H
#define Button_H
#include <Arduino.h>

const int INTERNAL_RESISTOR = INPUT_PULLUP;
const int EXTERNAL_RESISTOR = INPUT;

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
    void beginButton (int, unsigned long, unsigned int, int);
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
