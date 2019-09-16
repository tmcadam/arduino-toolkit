#include "flash.h"

void Flash::setPin(int _pinNumber){
  pinNumber = _pinNumber;
  pinMode(pinNumber, OUTPUT);
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
  if (flashInProgress == 0) {
      flashInProgress = 1;
      flashPeriod = _flashPeriod;
      numFlashes = _numFlashes;
      flashCounter = 0;
  }
  ndWatcher();
}

void Flash::ndWatcher() {
  if (flashInProgress >= 1) {
    unsigned long currentMillis = millis();
    if (flashInProgress == 1) {
      digitalWrite(pinNumber, HIGH);
      flashStartTime = currentMillis;
      flashInProgress = 2;
    } else if (currentMillis - flashStartTime >= flashPeriod and flashInProgress == 2) {
      digitalWrite(pinNumber, LOW);
      flashInProgress = 3;
    } else if (currentMillis - flashStartTime >= flashPeriod * 2 and flashInProgress == 3) {
      flashCounter++;
      if (flashCounter < numFlashes){
        flashInProgress = 1;
      }
      flashInProgress = 0;
    }
  }
}
