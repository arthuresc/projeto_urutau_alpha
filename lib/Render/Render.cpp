#include "Render.h"

Render::Render(Display& disp) : display(&disp), precisaAtualizar(true) {}

void Render::carregar(const DadosTela& dados) {
    // Compara com os últimos dados renderizados
    if (dados.titulo != ultimosDados.titulo || dados.linhas.size() != ultimosDados.linhas.size()) {
        precisaAtualizar = true;
    } else {
        for (size_t i = 0; i < dados.linhas.size(); i++) {
            if (dados.linhas[i].rotulo != ultimosDados.linhas[i].rotulo ||
                dados.linhas[i].valor  != ultimosDados.linhas[i].valor) {
                precisaAtualizar = true;
                break;
            }
        }
    }

    if (precisaAtualizar) {
        ultimosDados = dados;
    }
}

void Render::desenhar() {
    if (!precisaAtualizar) return;
    precisaAtualizar = false;

    // Converte os dados para o formato aceito pelo Display
    String titulo = ultimosDados.titulo;
    String mensagem = "";

    for (size_t i = 0; i < ultimosDados.linhas.size(); i++) {
        if (i > 0) mensagem += "\n";
        mensagem += ultimosDados.linhas[i].rotulo + ": " + ultimosDados.linhas[i].valor;
    }

    // Usa o método existente da classe Display
    display->update(titulo, mensagem);
}