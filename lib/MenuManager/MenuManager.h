#pragma once
#include <Arduino.h>
#include <functional>
#include <vector>

struct MenuItem {
    String titulo;
    std::function<void()> callback;
    std::vector<MenuItem> submenu;
};

class MenuManager {
private:
    MenuItem* raiz;
    MenuItem* menuAtual;
    std::vector<MenuItem*> caminho;
    int indiceSelecionado;
    unsigned long ultimoDebounceDisplay;

    std::vector<MenuItem> raizDinamica; // armazena os itens da raiz (para garantir vida útil)

public:
    MenuManager();
    void construirMenu(std::vector<MenuItem>& itensRaiz);
    void navegar(int direcao);
    void selecionar();
    void voltar();
    void atualizarDisplay();
    String getTituloAtual() const;
    String getTituloItemAtual() const;
    int getTamanhoSubmenu() const;
    int getIndiceSelecionado() const;
};