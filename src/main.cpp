#include "Button.h" // Inclui a biblioteca Button
#include "Led.h"    // Inclui a biblioteca Led
#include <Arduino.h>


// Uso
Button myButton(25);
Led myLed(2);


void setup() {
  myButton.begin();
  myLed.begin();
}

void loop() {
  if (myButton.wasPressed()) {
    myLed.blink();
  }
  
}