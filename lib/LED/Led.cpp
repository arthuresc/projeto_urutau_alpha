#include "Led.h"

Led::Led(int ledPin)
: state(LOW), pin(ledPin) {}

  
  void Led::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
  }
  
  void Led::toggle() {
    state = !state;
    digitalWrite(pin, state);
  }
  
  void Led::blink() {
    digitalWrite(pin, state);
    delay(200);
    digitalWrite(pin, !state);
    delay(200);
    digitalWrite(pin, state);
    delay(200);
    digitalWrite(pin, !state);
    delay(200);
    digitalWrite(pin, state);
    delay(200);
    digitalWrite(pin, !state);
    delay(200);
    digitalWrite(pin, state);
    delay(200);
    digitalWrite(pin, !state);
    delay(200);
    digitalWrite(pin, state);
  }

  void Led::set(bool newState) {
    state = newState;
    digitalWrite(pin, state);
  }
