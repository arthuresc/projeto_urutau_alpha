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
    secoes.clear();
    String secaoAtual = "";  // global

    while (arquivo.available()) {
        String linha = arquivo.readStringUntil('\n');
        linha.trim();
        if (linha.length() == 0 || linha[0] == '#') continue;

        // Detecta seção: [NOME]
        if (linha.startsWith("[") && linha.endsWith("]")) {
            secaoAtual = linha.substring(1, linha.length() - 1);
            secaoAtual.trim();
            if (secoes.find(secaoAtual) == secoes.end()) {
                secoes[secaoAtual] = std::map<String, String>();
            }
            continue;
        }

        int separador = linha.indexOf('=');
        if (separador > 0) {
            String chave = linha.substring(0, separador);
            String valor = linha.substring(separador + 1);
            chave.trim();
            valor.trim();

            if (secaoAtual.length() == 0) {
                dados[chave] = valor;          // parâmetro global
            } else {
                secoes[secaoAtual][chave] = valor;  // dentro da seção
            }
        }
    }
    arquivo.close();
    carregado = true;
    Serial.println("[Config] Arquivo carregado com " + String(dados.size()) + " param. globais e " + String(secoes.size()) + " secoes.");
    return true;
}

// ========== Métodos Globais (originais) ==========
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

// ========== Novos métodos de seção ==========
String Configuracao::getSecao(const String& secao, const String& chave, const String& padrao) const {
    auto itSecao = secoes.find(secao);
    if (itSecao != secoes.end()) {
        auto itChave = itSecao->second.find(chave);
        if (itChave != itSecao->second.end()) {
            return itChave->second;
        }
    }
    return padrao;
}

int Configuracao::getIntSecao(const String& secao, const String& chave, int padrao) const {
    String val = getSecao(secao, chave, "");
    if (val.length() == 0) return padrao;
    return val.toInt();
}

std::vector<String> Configuracao::listarSecoes() const {
    std::vector<String> lista;
    for (auto& par : secoes) lista.push_back(par.first);
    return lista;
}

bool Configuracao::estaCarregado() const { return carregado; }