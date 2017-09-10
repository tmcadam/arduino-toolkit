#include <Arduino.h>
#include <flash.h>

Flash f;
void setup() {
    f.setPin(9);
    f.dFlash(100, 5);
}

void loop() {
    f.ndWatcher();
    f.ndFlash(1000, 1);
}
