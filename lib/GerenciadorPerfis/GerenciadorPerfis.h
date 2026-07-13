#pragma once
#include <Arduino.h>
#include <vector>
#include "Configuracao.h"

struct DadosPerfil {
    String nome;
    int horaOn, minutoOn, horaOff, minutoOff;
    unsigned long duracaoRegaMs;
    std::vector<int> minutosRega;  // minutos do dia (0-1439)
};

class GerenciadorPerfis {
private:
    std::vector<DadosPerfil> perfis;
    Configuracao* config;
    bool sdDisponivel;

    // Carrega os perfis padrão (hardcoded) usados quando não há SD
    void carregarPadroes();

public:
    GerenciadorPerfis(Configuracao& cfg);
    void carregar();   // tenta carregar do SD; se falhar, carrega padrões
    size_t quantidade() const;
    DadosPerfil* obter(size_t indice);
    DadosPerfil* obterPorNome(const String& nome);
    String nomeAtivo;  // último perfil aplicado
};