#pragma once

#include <Arduino.h>

class Led {
  private:
    int pin;
    bool state;

  public:
    Led(int ledPin);

    void set(bool newState);

    void begin();

    void blink();

    void toggle();
};