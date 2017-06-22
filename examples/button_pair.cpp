#include "button.h"

Button redButton, blueButton;
ButtonPair buttonPair;
PairResult buttonPairEvent;

void setup() {
  // initialize the pushbutton pin as an input
  redButton.beginButton(54, 3000, 50, EXTERNAL_RESISTOR);
  blueButton.beginButton(55, 3000, 50, EXTERNAL_RESISTOR);
  buttonPair.setPair(redButton, blueButton);
  Serial.begin(115200);
  Serial.println("Button test sketch......");
}

void loop() {
  // read the state of the pushbutton value:

  buttonPairEvent = buttonPair.checkPair();
  if (buttonPairEvent.pairStatus == 1) {
      Serial.println("Both buttons pressed");
  }
  if (buttonPairEvent.status1 == 1)
  {
    Serial.println("Red button pressed");
  }
  if (buttonPairEvent.status1 == 2)
  {
    Serial.println("Red button long pressed");
  }
  if (buttonPairEvent.status2 == 1)
  {
    Serial.println("Blue button pressed");
  }
  if (buttonPairEvent.status2 == 2)
  {
    Serial.println("Blue button long pressed");
  }
}
