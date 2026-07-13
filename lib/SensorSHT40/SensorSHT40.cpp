#include "SensorSHT40.h"

SensorSHT40::SensorSHT40(TwoWire* i2cBus)
    : wire(i2cBus), temperatura(0), umidade(0), ativo(false) {
}

bool SensorSHT40::iniciar() {
    // O endereço fixo do SHT40 é 0x44
    if (!sht.begin(wire)) {
        Serial.println("[SHT40] Nao encontrado no barramento.");
        ativo = false;
        return false;
    }
    sht.setPrecision(SHT4X_HIGH_PRECISION);
    sht.setHeater(SHT4X_NO_HEATER);
    ativo = true;
    Serial.println("[SHT40] Inicializado.");
    return true;
}

void SensorSHT40::atualizar() {
    if (!ativo) return;
    sensors_event_t hum, temp;
    if (sht.getEvent(&hum, &temp)) {
        temperatura = temp.temperature;
        umidade = hum.relative_humidity;
    }
}

float SensorSHT40::getTemperatura() const { return temperatura; }
float SensorSHT40::getUmidade() const { return umidade; }
bool SensorSHT40::estaAtivo() const { return ativo; }