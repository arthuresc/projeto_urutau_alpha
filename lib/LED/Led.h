#pragma once

#include <Arduino.h>

class Led {
  private:
    uint8_t pin;
    bool state;

  public:
    explicit Led(uint8_t pin);
    void set(bool newState);
    // void blink();
    void toggle();
    void on();
    void off();
};