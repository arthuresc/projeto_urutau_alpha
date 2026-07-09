#pragma once
#include <Arduino.h>
#include <vector>
#include "Rele.h"
#include "Led.h"
#include "Button.h"
#include "Display.h"
#include "MenuManager.h"
#include "Render.h"
#include "DataLogger.h"
#include "Configuracao.h"
#include <Wire.h>

// --------------------------------------------
// Perfis de cultivo
// --------------------------------------------
enum FaseCultivo {
    GERMINACAO,
    VEGETATIVO,
    FLORACAO
};

// --------------------------------------------
// Estrutura para animações (placeholder)
// --------------------------------------------
struct Animacao {
    const uint8_t** frames;   // ponteiro para array de bitmaps
    int num_frames;
    int largura;
    int altura;
};


// --------------------------------------------
// Classe principal
// --------------------------------------------
class Sistema {
private:
    // --- Hardware básico ---
    Led ledInterno;
    Button btUp, btDown, btEnter;
    Display display;
    Render render;

    // --- Atuadores ---
    Rele luz;
    Rele bombaRega;

    // --- Interface ---
    MenuManager menu;
    unsigned long ultimoDisplay;

    // --- DataLogger e Configuração ---
    DataLogger logger;
    Configuracao config;

    // --- Estado do sistema ---
    FaseCultivo faseAtual;
    bool luzLigada;
    bool regaEmAndamento;
    bool modoManual;   // true = controle manual, false = controle pelo perfil

    // --- Ciclo de luz (horários fixos do perfil) ---
    uint8_t horaOn, minutoOn;
    uint8_t horaOff, minutoOff;

    // --- Ciclo de rega flexível ---
    std::vector<int> minutosRega;   // minutos do dia (0..1439)
    int ultimoMinutoRega;           // evita repetir no mesmo minuto
    unsigned long tempoInicioRega;  // millis() quando iniciou a rega
    unsigned long duracaoRegaMs;

    // --- Animações (placeholder) ---
    Animacao* animAtual;
    unsigned long tempoInicioAnim;
    int frameAtualAnim;
    bool animacaoEmExecucao;

    // --- Métodos internos ---
    void aplicarPerfil(FaseCultivo fase);
    void atualizarCicloLuz();
    void atualizarCicloRega();
    void atualizarDisplaySistema();
    void iniciarAnimacao(Animacao& anim);
    void atualizarAnimacao();
    void salvarEstado();
    void restaurarEstado();

public:
    Sistema();
    void iniciar();
    void atualizar();

    // Callbacks do menu
    void acaoModoManual();
    void acaoLigarLuz();
    void acaoDesligarLuz();
    void acaoRegarAgora();
    void acaoPerfilVegetacao();
    void acaoPerfilFloracao();
    void acaoPerfilGerminacao();
};