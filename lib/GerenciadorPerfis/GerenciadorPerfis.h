// #pragma once
// #include <Arduino.h>
// #include <vector>
// #include "Configuracao.h"

// struct DadosPerfil {
//     String nome;
//     int horaOn, minutoOn, horaOff, minutoOff;
//     unsigned long duracaoRegaMs;
//     std::vector<int> minutosRega;  // minutos do dia (0-1439)
// };

// class GerenciadorPerfis {
// private:
//     std::vector<DadosPerfil> perfis;
//     Configuracao* config;
// public:
//     GerenciadorPerfis(Configuracao& cfg);
//     void carregar();  // lê todas as seções do config.txt
//     size_t quantidade() const;
//     DadosPerfil* obter(size_t indice);
//     DadosPerfil* obterPorNome(const String& nome);
//     String nomeAtivo;  // último perfil aplicado
// };