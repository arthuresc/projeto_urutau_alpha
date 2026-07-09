#pragma once
#include <Arduino.h>
#include <SD.h>
#include <map>
#include <vector>

class Configuracao {
private:
    std::map<String, String> dados;                     // parâmetros globais (fora de seções)
    std::map<String, std::map<String, String>> secoes;  // seções: [VEGETATIVO], etc.
    bool carregado;

public:
    Configuracao();
    bool carregarDoSD(const String& caminho = "/config.txt");

    // Métodos globais (originais)
    String get(const String& chave, const String& padrao = "") const;
    int getInt(const String& chave, int padrao = 0) const;

    // Novos métodos para acessar chaves dentro de uma seção
    String getSecao(const String& secao, const String& chave, const String& padrao = "") const;
    int getIntSecao(const String& secao, const String& chave, int padrao = 0) const;

    std::vector<String> listarSecoes() const;

    bool estaCarregado() const;
};