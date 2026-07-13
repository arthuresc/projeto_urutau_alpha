#include "MenuManager.h"

MenuManager::MenuManager() 
    : raiz(nullptr), menuAtual(nullptr), indiceSelecionado(0), ultimoDebounceDisplay(0) {}

void MenuManager::setRaiz(MenuItem& item) {
    raiz = &item;
    menuAtual = &item;
    caminho.clear();
    indiceSelecionado = 0;
}

void MenuManager::navegar(int direcao) {
    if (menuAtual == nullptr || menuAtual->submenu.empty()) return;

    int tamanho = menuAtual->submenu.size();
    indiceSelecionado = (indiceSelecionado + direcao + tamanho) % tamanho;
    // if (direcao > 0){
    //     indiceSelecionado = (indiceSelecionado + direcao + tamanho) % tamanho;
    // }else if (direcao < 0) {
    // }
}

void MenuManager::selecionar() {
    if (menuAtual == nullptr) return;

    MenuItem& item = menuAtual->submenu[indiceSelecionado];

    // Se tem submenu, navega para dentro
    if (!item.submenu.empty()) {
        caminho.push_back(menuAtual);
        menuAtual = &item;
        indiceSelecionado = 0;
    } 
    // Se é folha, executa o callback
    else if (item.callback) {
        item.callback();
    }
}

void MenuManager::voltar() {
    if (!caminho.empty()) {
        menuAtual = caminho.back();
        caminho.pop_back();
        indiceSelecionado = 0;
    }
}

void MenuManager::atualizarDisplay() {
    // Futuramente, aqui vamos renderizar no display OLED
    // Por enquanto, apenas imprime no Serial para depuração

    unsigned long agora = millis();
    if (agora - ultimoDebounceDisplay < 300) return;
    ultimoDebounceDisplay = agora;

    Serial.println("=== MENU ===");
    if (menuAtual == nullptr) return;

    for (int i = 0; i < menuAtual->submenu.size(); i++) {
        if (i == indiceSelecionado) Serial.print(" > ");
        else Serial.print("   ");
        Serial.println(menuAtual->submenu[i].titulo);
    }
    Serial.println("===========");
}

String MenuManager::getTituloAtual() const {
    return (menuAtual != nullptr) ? menuAtual->titulo : "";
}

String MenuManager::getTituloItemAtual() const {
    if (menuAtual == nullptr || menuAtual->submenu.empty()) return "";
    if (indiceSelecionado >= menuAtual->submenu.size()) return "";
    return menuAtual->submenu[indiceSelecionado].titulo;
}

int MenuManager::getTamanhoSubmenu() const {
    return (menuAtual != nullptr) ? menuAtual->submenu.size() : 0;
}

int MenuManager::getIndiceSelecionado() const {
    return indiceSelecionado;
}

void MenuManager::setMenuDinamico(MenuItem& raiz, std::vector<MenuItem>& itens) {
    // Move os itens do vetor para o membro interno (para manter a referência válida)
    itensDinamicos = std::move(itens);
    // A raiz deve apontar para os itens armazenados em itensDinamicos
    // Isso exige que a raiz já tenha seu submenu configurado com os ponteiros corretos.
    // Vamos fazer a raiz ser um item cujo submenu é o próprio itensDinamicos.
    // Para simplificar, modificaremos a estrutura do menu em Sistema.
    setRaiz(raiz);
}