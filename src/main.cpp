#include <Arduino.h>
#include "Button.h"
#include "Led.h"
#include "Display.h"
#include "Rele.h"
#include "MenuManager.h"

// ----- Hardware -----
#define PIN_LED        2
#define PIN_BOTAO_UP   13
#define PIN_BOTAO_DOWN 14
#define PIN_BOTAO_ENTER 15

// ----- Componentes -----
Button btDown(PIN_BOTAO_DOWN);
Button btUp(PIN_BOTAO_UP);
Button btEnter(PIN_BOTAO_ENTER);
Led ledInterno(PIN_LED);
Display display;
Rele luz(4);   // Substitua pelo pino do relé da luz
MenuManager menu;

// ----- Callbacks das ações -----
void acaoLigarLuz() {
    luz.ligar();
    Serial.println("Ação: Ligar Luz");
    display.update("Menu", "Luz LIGADA");
}

void acaoDesligarLuz() {
    luz.desligar();
    Serial.println("Ação: Desligar Luz");
    display.update("Menu", "Luz DESLIGADA");
}

void acaoPiscarLed() {
    // Pisca rápido o LED interno apenas uma vez
    ledInterno.on();
    delay(200);
    ledInterno.off();
    Serial.println("Ação: Piscar LED");
    display.update("Menu", "Piscou LED");
}

// ----- Menu (estrutura) -----
MenuItem menuLuz[] = {
    {"LIGAR LUZ", acaoLigarLuz, {}},
    {"DESLIGAR LUZ", acaoDesligarLuz, {}}
};

MenuItem menuTeste[] = {
    {"PISCAR LED", acaoPiscarLed, {}}
};

MenuItem menuRaiz[] = {
    {"LUZ", nullptr, {menuLuz[0], menuLuz[1]}},
    {"TESTE", nullptr, {menuTeste[0]}}
};

// ----- Variáveis de tempo -----
unsigned long ultimoDisplay = 0;
const unsigned long intervaloDisplay = 500;

void setup() {
    Serial.begin(9600);
    Serial.println("Sistema com Menu - Teste");

    display.init();

    // Cria o menu raiz (contêiner)
    static MenuItem raizContainer = {"HOME", nullptr, {menuRaiz[0], menuRaiz[1]}};
    menu.setRaiz(raizContainer);

    // Exibe estado inicial
    menu.atualizarDisplay();
}

void loop() {
    // Leitura não bloqueante dos botões
    if (btUp.wasPressed()) {
        Serial.println("APERTOU BOTAO AZUL");
        menu.navegar(-1);
        menu.atualizarDisplay();
    }
    if (btDown.wasPressed()) {
        menu.navegar(1);
        menu.atualizarDisplay();
    }
    if (btEnter.wasPressed()) {
        menu.selecionar();
        menu.atualizarDisplay();
    }

    // Atualiza display periodicamente
    unsigned long agora = millis();
    if (agora - ultimoDisplay >= intervaloDisplay) {
        ultimoDisplay = agora;

        // Mostra no display o título do menu atual e o item selecionado
        String titulo = menu.getTituloAtual();
        int idx = menu.getIndiceSelecionado();
        int tam = menu.getTamanhoSubmenu();

        if (tam > 0) {
            // Mostra o título do item selecionado no momento (precisa de um método extra, faremos de forma simples)
            display.update(titulo, "Item " + String(idx+1) + "/" + String(tam));
        } else {
            display.update(titulo, "Sem opcoes");
        }
    }
}