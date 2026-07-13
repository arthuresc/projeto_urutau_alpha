#include "GerenciadorPerfis.h"

// Helper function to parse hours string e.g. "08:00,12:00" to minutes of day
static std::vector<int> parseHorarios(const String& horariosStr) {
    std::vector<int> minutos;
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
            minutos.push_back(h * 60 + m);
        }
        pos = sep + 1;
    }
    return minutos;
}

GerenciadorPerfis::GerenciadorPerfis(Configuracao& cfg) : config(&cfg), sdDisponivel(false), nomeAtivo("") {}

void GerenciadorPerfis::carregar() {
    perfis.clear();
    if (config && config->estaCarregado()) {
        std::vector<String> secoes = config->listarSecoes();
        for (auto& nome : secoes) {
            DadosPerfil p;
            p.nome = nome;
            p.horaOn   = config->getIntSecao(nome, "HORA_ON", 6);
            p.minutoOn = config->getIntSecao(nome, "MINUTO_ON", 0);
            p.horaOff  = config->getIntSecao(nome, "HORA_OFF", 24);
            p.minutoOff= config->getIntSecao(nome, "MINUTO_OFF", 0);
            p.duracaoRegaMs = (unsigned long)config->getIntSecao(nome, "DURACAO_REGA_MS", 5000);
            
            String horarios = config->getSecao(nome, "HORARIOS_REGA", "");
            p.minutosRega = parseHorarios(horarios);
            perfis.push_back(p);
        }
        sdDisponivel = !perfis.empty();
    } else {
        sdDisponivel = false;
    }

    if (perfis.empty()) {
        carregarPadroes();
    }
    Serial.println("Perfis carregados: " + String(perfis.size()));
}

void GerenciadorPerfis::carregarPadroes() {
    DadosPerfil germinacao;
    germinacao.nome = "GERMINACAO";
    germinacao.horaOn = 6;
    germinacao.minutoOn = 0;
    germinacao.horaOff = 24;
    germinacao.minutoOff = 0;
    germinacao.duracaoRegaMs = 5000;
    germinacao.minutosRega = parseHorarios("07:00,09:00,11:00,13:00,15:00,17:00");
    perfis.push_back(germinacao);

    DadosPerfil vegetativo;
    vegetativo.nome = "VEGETATIVO";
    vegetativo.horaOn = 6;
    vegetativo.minutoOn = 0;
    vegetativo.horaOff = 24;
    vegetativo.minutoOff = 0;
    vegetativo.duracaoRegaMs = 5000;
    vegetativo.minutosRega = parseHorarios("08:00,12:00,16:00,20:00");
    perfis.push_back(vegetativo);

    DadosPerfil floracao;
    floracao.nome = "FLORACAO";
    floracao.horaOn = 6;
    floracao.minutoOn = 0;
    floracao.horaOff = 24;
    floracao.minutoOff = 0;
    floracao.duracaoRegaMs = 5000;
    floracao.minutosRega = parseHorarios("08:00,14:00,20:00");
    perfis.push_back(floracao);
}

size_t GerenciadorPerfis::quantidade() const {
    return perfis.size();
}

DadosPerfil* GerenciadorPerfis::obter(size_t indice) {
    if (indice < perfis.size()) {
        return &perfis[indice];
    }
    return nullptr;
}

DadosPerfil* GerenciadorPerfis::obterPorNome(const String& nome) {
    for (auto& p : perfis) {
        if (p.nome.equalsIgnoreCase(nome)) {
            return &p;
        }
    }
    return nullptr;
}