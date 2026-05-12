#include "Led.h"

Led::Led(uint8_t pin): pin(pin), state(false) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

  
void Led::toggle() {
  state = !state;
  digitalWrite(pin, state);
}

void Led::set(bool newState) {
  state = newState;
  digitalWrite(pin, state);
}

void on() {
  digitalWrite(pin, HIGH);
}

