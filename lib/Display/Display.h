// #define BUTTON_H
// #ifdef BUTTON_H

#pragma once

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// #define OLED_RESET -1       // Pino de reset (-1 se não usado)
// #define I2C_ADDRESS 0x3C

class Display {
  private:
    uint8_t pin;
    uint8_t SCREEN_WIDTH;
    uint8_t SCREEN_HEIGHT;
    uint8_t OLED_RESET;
    uint8_t I2C_ADDRESS;

  public:
    Display();

    begin();

    void textoCentralizado(String texto, int tamanhoTexto);

    // bool isPressed();

    // bool readRaw();
};

// #endif