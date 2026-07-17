#include "SensorSHT40.h"

SensorSHT40::SensorSHT40(TwoWire* i2cBus, uint8_t address, const String& sensorName)
    : wire(i2cBus), temperatura(NAN), umidade(NAN), ativo(false), lastReadMillis(0), readIntervalMs(1000), nome(sensorName), i2cAddress(address) {
}

bool SensorSHT40::iniciar(uint8_t attempts, unsigned long retryDelayMs) {
    // tenta inicializar algumas vezes para tolerância
    for (uint8_t i = 0; i < attempts; ++i) {
        if (sht.begin(wire)) {
            // configurar precisão e heater
            sht.setPrecision(SHT4X_HIGH_PRECISION);
            sht.setHeater(SHT4X_NO_HEATER);
            ativo = true;
            Serial.println("[" + nome + "] Inicializado.");
            return true;
        }
        Serial.println("[" + nome + "] Tentativa " + String(i+1) + " falhou. Retentando...");
        delay(retryDelayMs);
    }
    Serial.println("[" + nome + "] Nao encontrado no barramento.");
    ativo = false;
    return false;
}

bool SensorSHT40::atualizar() {
    if (!ativo) return false;
    unsigned long now = millis();
    if (now - lastReadMillis < readIntervalMs) return false; // não é hora de ler

    sensors_event_t hum, temp;
    if (sht.getEvent(&hum, &temp)) {
        // validações simples
        float t = temp.temperature;
        float h = hum.relative_humidity;
        if (!isnan(t) && t > -40 && t < 125 && !isnan(h) && h >= 0 && h <= 100) {
            temperatura = t;
            umidade = h;
            lastReadMillis = now;
            return true;
        } else {
            Serial.println("[" + nome + "] Leitura invalida: T=" + String(t) + ", H=" + String(h));
            // Não desativa o sensor, apenas não atualiza valores
            lastReadMillis = now;
            return false;
        }
    } else {
        Serial.println("[" + nome + "] Falha ao obter evento do sensor.");
        // marca inativo para forçar reinit posterior
        ativo = false;
        return false;
    }
}

bool SensorSHT40::selfTest() {
    if (!iniciar(3, 100)) return false;
    // força uma leitura imediata (ignorando intervalo)
    unsigned long oldInterval = readIntervalMs;
    readIntervalMs = 0;
    bool ok = atualizar();
    readIntervalMs = oldInterval;
    if (ok) {
        Serial.println("[" + nome + "] Self-test OK: T=" + String(temperatura) + " C, H=" + String(umidade) + "%");
    } else {
        Serial.println("[" + nome + "] Self-test falhou.");
    }
    return ok;
}

float SensorSHT40::getTemperatura() const { return temperatura; }
float SensorSHT40::getUmidade() const { return umidade; }
bool SensorSHT40::estaAtivo() const { return ativo; }