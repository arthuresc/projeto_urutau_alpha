#include "Rele.h"

Rele::Rele(uint8_t pino, bool invertido) : pino(pino), invertido(invertido), estado(false) {
    pinMode(pino, OUTPUT);
    aplicarEstado(); // inicia desligado (estado = false)
}

void Rele::aplicarEstado() {
    if (invertido) {
        // Modo invertido: HIGH = ligado, LOW = desligado
        digitalWrite(pino, estado ? HIGH : LOW);
    } else {
        // Modo padrão: LOW = ligado, HIGH = desligado
        digitalWrite(pino, estado ? LOW : HIGH);
    }
}

void Rele::ligar() {
  estado = true;
  aplicarEstado();
}

void Rele::desligar() {
  estado = false;
  aplicarEstado();
}

void Rele::inverter() {
  if (estado) desligar();
  else ligar();
}

bool Rele::estaLigado() const {
  return estado;
}