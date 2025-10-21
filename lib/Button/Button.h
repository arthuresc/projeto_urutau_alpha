// #define BUTTON_H
// #ifdef BUTTON_H

#pragma once

#include <Arduino.h>

class Button {
  private:
    uint8_t pin;
    bool lastStableState;
    bool lastRawState;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;

  public:
    Button(uint8_t buttonPin, unsigned long debounceMs = 50);

    void begin();

    bool wasPressed();

    bool isPressed();

    bool readRaw();
};

// #endif