#include "Button.h"

Button::Button(uint8_t buttonPin, unsigned long debounceMs)
  : pin(buttonPin), 
    lastStableState(HIGH), 
    lastRawState(HIGH), 
    lastDebounceTime(0), 
    debounceDelay(debounceMs){}

    void Button::begin() {
      pinMode(pin, INPUT_PULLUP);
      lastStableState = digitalRead(pin);
      lastRawState = lastStableState;
    }

    bool Button::wasPressed() {
      bool currentReading = digitalRead(pin);
      bool pressed = false;

      if(currentReading != lastRawState) {
        lastDebounceTime = millis();
      }

      if ((millis() - lastDebounceTime) > debounceDelay) {
        if(currentReading != lastStableState) {
          if (lastStableState == HIGH && currentReading == LOW){
            pressed = true;
          }
          lastStableState = currentReading;
        }
      }

      lastRawState = currentReading;

      return pressed;
    }


  bool Button::isPressed() {
    bool currentReading = digitalRead(pin); 
    
    if (currentReading != lastRawState) {
      lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (currentReading != lastStableState) {
        lastStableState = currentReading;
      }
    }
    
    lastRawState = currentReading;

    return (lastStableState == LOW); // LOW = pressionado (PULLUP)
  }

bool Button::readRaw() {
  return digitalRead(pin);
}
  