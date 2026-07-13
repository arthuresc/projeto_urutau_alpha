#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT4x.h>

class SensorSHT40 {
private:
    Adafruit_SHT4x sht;
    TwoWire* wire;      // ponteiro para o barramento (Wire ou Wire1)
    float temperatura;
    float umidade;
    bool ativo;

public:
    // Construtor: recebe o ponteiro para o TwoWire (padrão = &Wire)
    SensorSHT40(TwoWire* i2cBus = &Wire);
    bool iniciar();
    void atualizar();
    float getTemperatura() const;
    float getUmidade() const;
    bool estaAtivo() const;
};