#pragma once
#include <Arduino.h>
#include <SD.h>
#include <map>
#include <vector>

class Configuracao {
private:
    std::map<String, String> dados;
    std::map<String, std::map<String, String>> secoes;
    bool carregado;

public:
    Configuracao();
    bool carregarDoSD(const String& caminho = "/config.txt");
    String get(const String& chave, const String& padrao = "") const;
    int getInt(const String& chave, int padrao = 0) const;
    String getSecao(const String& secao, const String& chave, const String& padrao = "") const;
    int getIntSecao(const String& secao, const String& chave, int padrao = 0) const;
    std::vector<String> listarSecoes() const;
    bool estaCarregado() const;
};