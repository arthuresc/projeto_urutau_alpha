#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include "Rele.h"
#include "Button.h"
#include "Display.h"
#include "MenuManager.h"
#include "Render.h"
#include "DataLogger.h"
#include "Configuracao.h"
#include "GerenciadorPerfis.h"
#include "SensorSHT40.h"
#include "SensorLuz.h"

class Sistema {
private:
    // Hardware
    Button btUp, btDown, btEnter;
    Display display;
    Render render;

    // Atuadores
    Rele luz;            // relé das lâmpadas (juntas)
    Rele coolerEntrada;
    Rele coolerInterno;
    Rele valvulaRega;
    // cooler exaustor é controlado por PWM separadamente

    // Sensores
    SensorSHT40 sensorInterno;   // I2C0
    SensorSHT40 sensorExterno;   // I2C1
    SensorLuz sensorLuz;
    // Sensores de solo (4 canais ADC)
    uint8_t pinosSolo[4] = {36, 39, 34, 35}; // GPIOs ADC1 (atenção: 34,35 apenas entrada, sem pull-up)

    // Módulos
    MenuManager menu;
    DataLogger logger;
    Configuracao config;
    GerenciadorPerfis gerenciador;

    // Estado
    String perfilAtivo;
    bool luzLigada;
    bool regaEmAndamento;
    unsigned long tempoInicioRega;
    unsigned long duracaoRegaMs;
    int horaOn, minutoOn, horaOff, minutoOff;
    std::vector<int> minutosRega;
    int ultimoMinutoRega;

    bool modoHUD;
    unsigned long ultimoDisplay;

    // Métodos internos
    void construirMenu();
    void aplicarPerfil(const String& nome);
    void atualizarCicloLuz();
    void atualizarCicloRega();
    void atualizarHUD();
    void atualizarDisplaySistema();

public:
    Sistema();
    void iniciar();
    void atualizar();
};