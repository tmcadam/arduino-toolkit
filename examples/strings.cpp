#include <Arduino.h>
#include <strings.h>

char valBuf[32];

void readTemp(char* buf){
  float tempf = float(random(15000, 35000)) / float(1000);
  Serial.println(tempf);
  floatToChar(buf, tempf);
}

void setup(void) {
  Serial.begin(9600);
  delay(2000);
}

void loop(void) {
  readTemp(valBuf);
  Serial.println(valBuf);
  delay(1000);
}
