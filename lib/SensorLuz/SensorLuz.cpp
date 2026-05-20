#include "SensorLuz.h"

SensorLuz::SensorLuz() : lux(0), ativo(false) {}

bool SensorLuz::iniciar() {
    // Modo de alta resolução contínua e endereço I2C padrão 0x23
    sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);
    if (sensor.readLightLevel() < 0) {
        Serial.println("[SensorLuz] ERRO: BH1750 nao encontrado");
        ativo = false;
        return false;
    }
    ativo = true;
    Serial.println("[SensorLuz] BH1750 iniciado");
    return true;
}

void SensorLuz::atualizar() {
    if (!ativo) return;
    lux = sensor.readLightLevel();
}

float SensorLuz::getLux() const { return lux; }

float SensorLuz::getPPFD() const {
    // Fator de conversão para LED full-spectrum
    return lux * 0.0185;
}

bool SensorLuz::estaAtivo() const { return ativo; }