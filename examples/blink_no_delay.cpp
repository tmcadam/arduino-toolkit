#include <Arduino.h>
#include <timer.h>

#define LED_PIN 13
Timer t;

void flash();

void setup() {
    pinMode(LED_PIN, OUTPUT);
    t.setPeriod(100);
    t.setAction(flash);
    // or
    t.setTimer(100, flash);
}

void loop() {
    t.update();
}

int c = 0;
void flash() {
    c++;
    byte state = digitalRead(LED_PIN);
    switch (state) {
        case HIGH:
            digitalWrite(LED_PIN, LOW);
            break;
        case LOW:
            digitalWrite(LED_PIN, HIGH);
            break;
    }
    if ( c >10 ) t.stop();
}
