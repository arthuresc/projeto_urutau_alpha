#pragma once
#include <Arduino.h>
#include <SD.h>
#include <map>

class Configuracao {
private:
    std::map<String, String> dados;
    bool carregado;

public:
    Configuracao();
    bool carregarDoSD(const String& caminho = "/config.txt");
    String get(const String& chave, const String& padrao = "") const;
    int getInt(const String& chave, int padrao = 0) const;
    bool estaCarregado() const;
};