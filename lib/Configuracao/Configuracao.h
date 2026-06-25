#pragma once
#include <Arduino.h>
#include <SD.h>
#include <map>

class Configuracao {
private:
    std::map<String, String> dados;                     // chaves globais (fora de seções)
    std::map<String, std::map<String, String>> secoes;  // seções do arquivo
    bool carregado;

public:
    Configuracao();
    bool carregarDoSD(const String& caminho = "/config.txt");
    String get(const String& chave, const String& padrao = "") const;
    int getInt(const String& chave, int padrao = 0) const;

    // Novo método para acessar chaves dentro de uma seção
    String getSecao(const String& secao, const String& chave, const String& padrao = "") const;
    int getIntSecao(const String& secao, const String& chave, int padrao = 0) const;

    bool estaCarregado() const;
};