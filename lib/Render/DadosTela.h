#pragma once
#include <Arduino.h>
#include <vector>

struct LinhaTela {
  String rotulo;
  String valor;
};

struct DadosTela {
  String titulo;
  std::vector<LinhaTela> linhas;
};