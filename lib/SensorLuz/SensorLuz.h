#pragma once
#include <Arduino.h>
#include <BH1750.h>

class SensorLuz {
private:
    BH1750 sensor;
    float lux;
    bool ativo;
public:
    SensorLuz();
    bool iniciar();
    void atualizar();
    float getLux() const;
    float getPPFD() const;   // Conversão para LED full-spectrum
    bool estaAtivo() const;
};