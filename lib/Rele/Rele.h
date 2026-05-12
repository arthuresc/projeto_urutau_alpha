#pragma once

#include <Arduino.h>

class Rele {
  private:
    uint8_t pino;
    bool estado;  // true = ligado, false = desligado

  public:
    explicit Rele(uint8_t pino); //construtor
    void ligar();
    void desligar();
    void inverter();
    bool estaLigado();
    
};