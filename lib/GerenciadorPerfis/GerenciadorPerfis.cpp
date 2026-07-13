#include "GerenciadorPerfis.h"

GerenciadorPerfis::GerenciadorPerfis(Configuracao& cfg) : config(&cfg) {}

bool GerenciadorPerfis::carregar() {
    if (!config->estaCarregado()) {
        Serial.println("[Perfis] Configuracao nao carregada.");
        return false;
    }
    std::vector<String> secoes = config->listarSecoes();
    if (secoes.empty()) {
        Serial.println("[Perfis] Nenhuma secao encontrada.");
        return false;
    }
    perfis.clear();
    for (auto& nome : secoes) {
        DadosPerfil p;
        p.nome = nome;
        p.horaOn   = config->getIntSecao(nome, "HORA_ON", 6);
        p.minutoOn = config->getIntSecao(nome, "MINUTO_ON", 0);
        p.horaOff  = config->getIntSecao(nome, "HORA_OFF", 24);
        p.minutoOff= config->getIntSecao(nome, "MINUTO_OFF", 0);
        p.duracaoRegaMs = (unsigned long)config->getIntSecao(nome, "DURACAO_REGA_MS", 5000);

        // Horários de rega
        String horariosStr = config->getSecao(nome, "HORARIOS_REGA", "");
        p.minutosRega.clear();
        if (horariosStr.length() > 0) {
            int pos = 0;
            while (pos < horariosStr.length()) {
                int sep = horariosStr.indexOf(',', pos);
                if (sep == -1) sep = horariosStr.length();
                String horario = horariosStr.substring(pos, sep);
                horario.trim();
                int doisPontos = horario.indexOf(':');
                if (doisPontos > 0) {
                    int h = horario.substring(0, doisPontos).toInt();
                    int m = horario.substring(doisPontos+1).toInt();
                    p.minutosRega.push_back(h * 60 + m);
                }
                pos = sep + 1;
            }
        }
        perfis.push_back(p);
    }
    Serial.println("[Perfis] Carregados: " + String(perfis.size()));
    return true;
}

size_t GerenciadorPerfis::quantidade() const { return perfis.size(); }

DadosPerfil* GerenciadorPerfis::obter(size_t indice) {
    if (indice < perfis.size()) return &perfis[indice];
    return nullptr;
}

DadosPerfil* GerenciadorPerfis::obterPorNome(const String& nome) {
    for (auto& p : perfis) if (p.nome == nome) return &p;
    return nullptr;
}