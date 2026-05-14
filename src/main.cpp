#include <Arduino.h>
#include "Button.h"
#include "Led.h"
#include "Display.h"
#include "Rele.h"
#include "MenuManager.h"
#include "Render.h"

// ----- Hardware -----
#define PIN_LED         2
#define PIN_BOTAO_UP    13
#define PIN_BOTAO_DOWN  14
#define PIN_BOTAO_ENTER 15

// ----- Componentes -----
Led ledInterno(PIN_LED);
Button btUp(PIN_BOTAO_UP);
Button btDown(PIN_BOTAO_DOWN);
Button btEnter(PIN_BOTAO_ENTER);
Display display;
Render render(display);
Rele luz(4);   // Substitua pelo pino do relé da luz
MenuManager menu;

// ----- Callbacks das ações -----
void acaoLigarLuz() {
    luz.ligar();
    Serial.println("Luz ligada");
}

void acaoDesligarLuz() {
    luz.desligar();
    Serial.println("Luz desligada");
}

void acaoRegarAgora() {
    // Simula abertura da válvula de rega com o LED interno
    Serial.println("Rega acionada por 2 segundos (simulacao)");
    ledInterno.on();
    delay(2000);
    ledInterno.off();
    Serial.println("Rega finalizada");
}

void acaoPerfilVegetacao() {
    Serial.println("Perfil Vegetacao aplicado (placeholder)");
}

void acaoPerfilFloracao() {
    Serial.println("Perfil Floracao aplicado (placeholder)");
}

void acaoPerfilGerminacao() {
    Serial.println("Perfil Germinacao aplicado (placeholder)");
}

// ----- Estrutura do Menu (árvore) -----
// Submenu ILUMINACAO
MenuItem submenuIluminacao[] = {
    {"LIGAR LUZ", acaoLigarLuz, {}},
    {"DESLIGAR LUZ", acaoDesligarLuz, {}},
    {"VOLTAR", [](){ menu.voltar(); }, {}}     // <-- novo item
};

// Submenu REGA
MenuItem submenuRega[] = {
    {"REGAR AGORA", acaoRegarAgora, {}},
    {"VOLTAR", [](){ menu.voltar(); }, {}}
};

// Submenu PERFIS
MenuItem submenuPerfis[] = {
    {"VEGETACAO", acaoPerfilVegetacao, {}},
    {"FLORACAO", acaoPerfilFloracao, {}},
    {"GERMINACAO", acaoPerfilGerminacao, {}},
    {"VOLTAR", [](){ menu.voltar(); }, {}}
};

MenuItem menuPrincipal[] = {
    {"HOME", nullptr, {}},
    {"ILUMINACAO", nullptr, {submenuIluminacao[0], submenuIluminacao[1], submenuIluminacao[2]}},
    {"REGA", nullptr, {submenuRega[0], submenuRega[1]}},
    {"PERFIS", nullptr, {submenuPerfis[0], submenuPerfis[1], submenuPerfis[2], submenuPerfis[3]}}
};

// ----- Variáveis de tempo -----
unsigned long ultimoDisplay = 0;
const unsigned long intervaloDisplay = 500;

void setup() {
    Serial.begin(9600);
    Serial.println("Sistema Grow Indoor - Menu Completo");
    
    display.init();
    
    // Cria o container raiz
    static MenuItem raizContainer = {"MENU PRINCIPAL", nullptr, {
        menuPrincipal[0], menuPrincipal[1], menuPrincipal[2], menuPrincipal[3]
    }};
    menu.setRaiz(raizContainer);
    
    // Exibe o menu no Serial para debug
    menu.atualizarDisplay();
}

void loop() {
    // Leitura dos botões
    if (btUp.wasPressed()) {
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
    
    // Atualiza o display OLED periodicamente
    unsigned long agora = millis();
    if (agora - ultimoDisplay >= intervaloDisplay) {
        ultimoDisplay = agora;
        
        DadosTela tela;
        String tituloAtual = menu.getTituloAtual();
        int idx = menu.getIndiceSelecionado();
        int tam = menu.getTamanhoSubmenu();
        
        // Lógica para exibir o item selecionado
        if (tam > 0) {
            // Acessamos o título do item selecionado via Serial (gambiarra temporária)
            // O ideal seria um método getTituloItem(idx), mas faremos uma exibição simples
            tela.titulo = tituloAtual;
            String nomeItem = menu.getTituloItemAtual();
            tela.linhas.push_back({"Selecione", nomeItem});
        } else {
            // Se não há submenu (folha ou vazio), mostra título e mensagem fixa
            tela.titulo = tituloAtual;
            tela.linhas.push_back({"", ""});  // limpa linha
        }
        
        // Tratamento especial para HOME
        if (tituloAtual == "HOME") {
            tela.titulo = "HOME";
            tela.linhas.clear();
            tela.linhas.push_back({"Grow Indoor", "Pronto"});
            tela.linhas.push_back({"Temp", "-- C"});
            tela.linhas.push_back({"Umid", "-- %"});
        }
        
        render.carregar(tela);
        render.desenhar();
    }
}