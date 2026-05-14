#pragma once
#include <Arduino.h>
#include <functional>
#include <vector>
#include <Display.h>

struct MenuItem {
  String titulo;
  std::function<void()> callback;
  std::vector<MenuItem> submenu;
};

class MenuManager {
  private: //Estrutura de navegação
    MenuItem* raiz;
    MenuItem* menuAtual;
    std::vector<MenuItem*> caminho; //pilha de navegação (histórico)

    //Interface
    int indiceSelecionado;
    unsigned long ultimoDebounceDisplay;

    public:
      MenuManager();
      void setRaiz(MenuItem& item);
      void navegar(int direcao);
      void selecionar();
      void voltar();
      void atualizarDisplay();
      String getTituloAtual() const;
      String getTituloItemAtual() const;   // retorna o título do item selecionado
      int getTamanhoSubmenu() const;
      int getIndiceSelecionado() const;
};