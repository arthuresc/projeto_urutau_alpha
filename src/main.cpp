#include <Arduino.h>
#include "Sistema.h"
#include "CoolerPWM.h"
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>

Sistema sistema;

CoolerPWM exaustor(16, 17); // pino 16, canal 0

// #define CS 5 // adicione outros se quiser

// RTC_DS3231 rtc;

void setup() {
  sistema.iniciar();
}

void loop() {
  sistema.atualizar();
}
