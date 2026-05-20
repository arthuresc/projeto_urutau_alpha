#include <Arduino.h>
#include "Sistema.h"

Sistema sistema;

void setup() {
    sistema.iniciar();
}

void loop() {
    sistema.atualizar();
}