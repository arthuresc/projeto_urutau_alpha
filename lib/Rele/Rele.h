#pragma once

#include <Arduino.h>

class Rele {
  private:
    uint8_t pino;
    bool estado;  // true = ligado, false = desligado
    bool invertido;     // true = lógica invertida (HIGH = ligado)
    void aplicarEstado(); // aplica o estado físico baseado na flag

  public:
    explicit Rele(uint8_t pino, bool invertido = false); //construtor
    void ligar();
    void desligar();
    void inverter();
    bool estaLigado() const;
    
};