#include "Button.h" // Inclui a biblioteca Button
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1       // Pino de reset (-1 se não usado)
#define I2C_ADDRESS 0x3C   // Endereço I2C comum para esses displays

const int LED_PIN = 2; // Pino do LED embutido (opcional)

// Inicializa o objeto do display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Uso
Button myButton(25);

class Led {
  private:
  int pin;
  bool state;
  
  public:
  Led(int ledPin) : pin(ledPin), state(LOW) {}
  
  void begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
  }
  
  void toggle() {
    state = !state;
    digitalWrite(pin, state);
  }
  
  void blink() {
    // state = !state;
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

  void set(bool newState) {
    state = newState;
    digitalWrite(pin, state);
  }
};

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