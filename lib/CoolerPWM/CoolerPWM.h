#pragma once
#include <Arduino.h>

class CoolerPWM {
private:
    uint8_t pino;
    uint8_t canalPWM;
    int velocidadeAtual; // 0 a 100

public:
    CoolerPWM(uint8_t pino, uint8_t canal = 0);
    void iniciar();
    void setVelocidade(int porcentagem);
    void desligar();
    int getVelocidade() const;
};