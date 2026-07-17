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

    unsigned long lastReadMillis;
    unsigned long readIntervalMs;
    String nome;
    uint8_t i2cAddress; // SHT4x pode ser 0x44 ou 0x45

public:
    // Construtor: recebe o ponteiro para o TwoWire (padrão = &Wire) e opcionalmente o endereço
    SensorSHT40(TwoWire* i2cBus = &Wire, uint8_t address = 0x44, const String& sensorName = "SHT4x");

    // Inicializa o sensor. Retorna true se inicializou com sucesso.
    // attempts: número de tentativas de comunicação antes de falhar
    bool iniciar(uint8_t attempts = 3, unsigned long retryDelayMs = 200);

    // Atualiza leituras (respeita readIntervalMs). Retorna true se houve nova leitura.
    bool atualizar();

    // Testa o sensor (lê uma vez e verifica valores plausíveis)
    bool selfTest();

    float getTemperatura() const;
    float getUmidade() const;
    bool estaAtivo() const;

    // Tempo da última leitura (millis())
    unsigned long getLastReadMillis() const { return lastReadMillis; }

    // Ajusta intervalo entre leituras (padrão 1000 ms)
    void setReadInterval(unsigned long ms) { readIntervalMs = ms; }

    String getNome() const { return nome; }
};