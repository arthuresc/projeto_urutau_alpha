#pragma once
#include <Arduino.h>
#include "DadosTela.h"
#include "Display.h"

class Render {
  private:
    Display* display;
    DadosTela ultimosDados; // cache para evitar redesenho desnecessário
    bool precisaAtualizar;

  public:
    Render(Display& disp);
    void carregar(const DadosTela& dados);
    void desenhar();
};
