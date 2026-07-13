#include "MenuManager.h"

MenuManager::MenuManager() : raiz(nullptr), menuAtual(nullptr), indiceSelecionado(0), ultimoDebounceDisplay(0) {}

void MenuManager::construirMenu(std::vector<MenuItem>& itensRaiz) {
    raizDinamica = std::move(itensRaiz);
    // Cria um nó raiz temporário que contém o vetor
    static MenuItem raizContainer; // static para persistir
    raizContainer.titulo = "RAIZ";
    raizContainer.submenu = raizDinamica;
    raiz = &raizContainer;
    menuAtual = raiz;
    indiceSelecionado = 0;
    caminho.clear();
}

void MenuManager::navegar(int direcao) {
    if (menuAtual == nullptr || menuAtual->submenu.empty()) return;
    int tamanho = menuAtual->submenu.size();
    indiceSelecionado = (indiceSelecionado + direcao + tamanho) % tamanho;
}

void MenuManager::selecionar() {
    if (menuAtual == nullptr) return;
    MenuItem& item = menuAtual->submenu[indiceSelecionado];
    if (!item.submenu.empty()) {
        caminho.push_back(menuAtual);
        menuAtual = &item;
        indiceSelecionado = 0;
    } else if (item.callback) {
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

void MenuManager::atualizarDisplay() { /* mesma implementação anterior, apenas debug Serial */ }

String MenuManager::getTituloAtual() const {
    return (menuAtual != nullptr) ? menuAtual->titulo : "";
}

String MenuManager::getTituloItemAtual() const {
    if (menuAtual == nullptr || menuAtual->submenu.empty()) return "";
    int idx = getIndiceSelecionado();
    if (idx >= 0 && idx < (int)menuAtual->submenu.size())
        return menuAtual->submenu[idx].titulo;
    return "";
}

int MenuManager::getTamanhoSubmenu() const {
    return (menuAtual != nullptr) ? menuAtual->submenu.size() : 0;
}

int MenuManager::getIndiceSelecionado() const { return indiceSelecionado; }