// #include "GerenciadorPerfis.h"

// GerenciadorPerfis::GerenciadorPerfis(Configuracao& cfg) : config(&cfg) {}

// void GerenciadorPerfis::carregar() {
//     perfis.clear();
//     // Percorre as seções conhecidas (precisamos de um método que liste as seções)
//     // Vamos expor a lista de seções na Configuracao. Adicione um método 'std::vector<String> listarSecoes()'
//     // que retorna as chaves do mapa 'secoes'.
//     std::vector<String> secoes = config->listarSecoes();
//     for (auto& nome : secoes) {
//         DadosPerfil p;
//         p.nome = nome;
//         p.horaOn   = config->getIntSecao(nome, "HORA_ON", 6);
//         p.minutoOn = config->getIntSecao(nome, "MINUTO_ON", 0);
//         p.horaOff  = config->getIntSecao(nome, "HORA_OFF", 24);
//         p.minutoOff= config->getIntSecao(nome, "MINUTO_OFF", 0);
//         p.duracaoRegaMs = (unsigned long)config->getIntSecao(nome, "DURACAO_REGA_MS", 5000);
        
//         String horarios = config->getSecao(nome, "HORARIOS_REGA", "");
//         // parse de minutos (igual ao código atual)
//         // ...
//         perfis.push_back(p);
//     }
//     Serial.println("Perfis carregados: " + String(perfis.size()));
// }
// // ... outros métodos