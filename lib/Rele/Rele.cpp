#include "Rele.h"

Rele::Rele(uint8_t pino) : pino(pino), estado(false) {
  pinMode(pino, OUTPUT); // veio do <Arduino.h> do nosso .h
  digitalWrite(pino, HIGH); //inicia desligado
}

void Rele::ligar() {
  digitalWrite(pino, LOW);
  estado = true;
}

void Rele::desligar() {
  digitalWrite(pino, HIGH);
  estado = false;
}

void Rele::inverter() {
  if (estado) desligar();
  else ligar();
}

bool Rele::estaLigado() {
  return estado;
}