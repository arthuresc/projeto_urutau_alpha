#include "Configuracao.h"

Configuracao::Configuracao() : carregado(false) {}

bool Configuracao::carregarDoSD(const String& caminho) {
    if (!SD.begin()) {
        Serial.println("[Config] SD nao inicializado.");
        carregado = false;
        return false;
    }

    File arquivo = SD.open(caminho, FILE_READ);
    if (!arquivo) {
        Serial.println("[Config] Arquivo " + caminho + " nao encontrado. Usando padroes.");
        carregado = false;
        return false;
    }

    dados.clear();
    while (arquivo.available()) {
        String linha = arquivo.readStringUntil('\n');
        linha.trim();
        // Ignora linhas em branco ou comentários (#)
        if (linha.length() == 0 || linha[0] == '#') continue;

        int separador = linha.indexOf('=');
        if (separador > 0) {
            String chave = linha.substring(0, separador);
            String valor = linha.substring(separador + 1);
            chave.trim();
            valor.trim();
            dados[chave] = valor;
        }
    }
    arquivo.close();
    carregado = true;
    Serial.println("[Config] Arquivo carregado com " + String(dados.size()) + " parametros.");
    return true;
}

String Configuracao::get(const String& chave, const String& padrao) const {
    if (dados.find(chave) != dados.end()) {
        return dados.at(chave);
    }
    return padrao;
}

int Configuracao::getInt(const String& chave, int padrao) const {
    String val = get(chave, "");
    if (val.length() == 0) return padrao;
    return val.toInt();
}

bool Configuracao::estaCarregado() const { return carregado; }