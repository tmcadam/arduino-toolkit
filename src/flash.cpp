#include "flash.h"

void Flash::setPin(int _pinNumber){
  this->pinNumber = _pinNumber;
  pinMode(this->pinNumber, OUTPUT);
}

void Flash::dFlash(unsigned long flashPeriod) {
    digitalWrite(pinNumber, HIGH);
    delay(flashPeriod);
    digitalWrite(pinNumber, LOW);
    delay(flashPeriod);
}

void Flash::dFlashes(unsigned long flashPeriod, int flashCount){
    int c = 0;
    while( c < flashCount ) {
        dFlash(flashPeriod);
        c++;
    }
}

void Flash::ndFlash(unsigned long _flashPeriod, int _numFlashes){
  if (this->flashInProgress == 0) {
      this->flashInProgress = 1;
      this->flashPeriod = _flashPeriod;
      this->numFlashes = _numFlashes;
      this->flashCounter = 0;
  }
  this->ndWatcher();
}

void Flash::ndWatcher() {
  if (this->flashInProgress >= 1) {
    unsigned long currentMillis = millis();
    if (this->flashInProgress == 1) {
      digitalWrite(this->pinNumber, HIGH);
      this->flashStartTime = currentMillis;
      this->flashInProgress = 2;
    } else if (currentMillis - this->flashStartTime >= this->flashPeriod and this->flashInProgress == 2) {
      digitalWrite(pinNumber, LOW);
      flashInProgress = 3;
    } else if (currentMillis - this->flashStartTime >= this->flashPeriod * 2 and this->flashInProgress == 3) {
      this->flashCounter++;
      if (this->flashCounter < this->numFlashes){
        this->flashInProgress = 1;
      }
      this->flashInProgress = 0;
    }
  }
}
