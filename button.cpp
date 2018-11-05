//button.cpp
#include "button.h"

void ButtonPair::setPair(Button button1, Button button2){
    this->button1 = button1;
    this->button2 = button2;
}

PairResult ButtonPair::checkPair(){
  int pairStatus = 0;
  int status1 = button1.checkButton();
  int status2 = button2.checkButton();
  if (status1 == 3 && status2 == 3) {
    button1.setState(3);
    button2.setState(3);
    pairStatus = 1;
  }
  PairResult result = { pairStatus, status1, status2 };
  return result;
}


#if defined(__STM32F1__)
  void Button::beginButton (int pinNumber, unsigned long longHoldThreshold, unsigned int checkDelay, WiringPinMode resistorType) {
#else
  void Button::beginButton (int pinNumber, unsigned long longHoldThreshold, unsigned int checkDelay, int resistorType) {
#endif
  this->pinNumber = pinNumber;
  this->longHoldThreshold = longHoldThreshold;
  this->lastCheck = millis();
  this->checkDelay = checkDelay;
  pinMode(pinNumber, resistorType);
}

void Button::getPosition () {
    this->currentPosition = digitalRead(this->pinNumber);
}

void Button::setState (int state){
    this->state = state;
}

int Button::checkButton () {

  //implements a delay to avoid bounce and things
  if ( (millis() - this->lastCheck) > this->checkDelay ) {

      //update value for last check
      this->lastCheck = millis();

      //read the button position
      this->getPosition();

      //check if the button has changed position
      if ( this->currentPosition != this->previousPosition ) {
          //update the state as the button has changed position
          this->previousPosition = this->currentPosition;

          //checks for button release after a press
          if (this->currentPosition == HIGH && this->state == 0){
              this->state = 1;
              return 1;
          } else if (this->currentPosition == LOW) {
              this->state = 0;
              this->longHoldStartTime = millis();
              return 3;
          }
      } else if ( (millis() - this->longHoldStartTime) > this->longHoldThreshold && this->currentPosition == LOW && this->state == 0 ) {
          this->state = 2;
          return 2;
      } else if ( this->currentPosition == LOW && this->state == 0 ) {
          return 3;
      }
  }
  return 0;
}
