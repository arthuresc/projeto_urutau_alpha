#include "Sistema.h"

Sistema::Sistema()
    : btUp(13), btDown(34), btEnter(15),
      display(), render(display),
      luz(12, false),          // relé lâmpadas (LOW ligado)
      coolerVentilacao(26, false),
      coolerExaustao(27, false),
      valvulaRega(14, false),
      sensorInterno(&Wire),    // I2C0
      sensorExterno(&Wire1),   // I2C1
      sensorLuz(),
      menu(), ultimoDisplay(0),
      config(), gerenciador(config),
      perfilAtivo(""), luzLigada(false), regaEmAndamento(false),
      tempoInicioRega(0), duracaoRegaMs(5000),
      horaOn(6), minutoOn(0), horaOff(24), minutoOff(0),
      ultimoMinutoRega(-1),
      modoHUD(false)
{
}

void Sistema::iniciar() {
    Serial.begin(9600);
    // 1. Inicializar barramentos I2C
    // Wire.begin(21, 22);      // I2C0 (display, sensores internos, RTC)
    Wire1.begin(33, 32);     // I2C1 (SHT40 externo)

    // 2. Display
    display.init();
    delay(100);

    // 3. Logger (RTC + SD)
    logger.iniciar();

    // 4. Carregar configuração do SD
    if (!config.carregarDoSD("/config.txt")) {
        Serial.println("[Sistema] config.txt nao encontrado. Sistema nao pode operar.");
        display.update("ERRO", "SD/config.txt");
        while (1) { delay(1000); }
    }

    // 5. Carregar perfis do SD
    if (!gerenciador.carregar()) {
        Serial.println("[Sistema] Nenhum perfil no config.txt.");
        display.update("ERRO", "Sem perfis");
        while (1) { delay(1000); }
    }

    // 6. Definir perfil ativo
    perfilAtivo = config.get("PERFIL_ATIVO", "");
    if (perfilAtivo.isEmpty() || !gerenciador.obterPorNome(perfilAtivo)) {
        // Usar o primeiro perfil carregado
        perfilAtivo = gerenciador.obter(0)->nome;
        Serial.println("[Sistema] Nenhum perfil ativo definido, usando primeiro: " + perfilAtivo);
    }

    // 7. Construir menu dinâmico
    construirMenu();

    // 8. Aplicar perfil ativo
    aplicarPerfil(perfilAtivo);

    // 9. Inicializar sensores (opcional, se falhar, não trava)
    sensorInterno.iniciar();
    sensorExterno.iniciar();
    sensorLuz.iniciar();
    // sensores de solo não precisam de init, apenas pinMode nos pinos (feito no construtor)
    for (int i=0; i<4; i++) pinMode(pinosSolo[i], INPUT);

    menu.atualizarDisplay();
}

void Sistema::construirMenu() {
    std::vector<MenuItem> itensPerfis;
    for (size_t i = 0; i < gerenciador.quantidade(); i++) {
        String nome = gerenciador.obter(i)->nome;
        itensPerfis.push_back({nome, [this, nome]() {
            aplicarPerfil(nome);
        }, {}});
    }
    itensPerfis.push_back({"VOLTAR", [this](){ menu.voltar(); }, {}});

    std::vector<MenuItem> raizItens = {
        {"HOME", nullptr, {}},
        {"HUD", [this](){ modoHUD = true; }, {}},
        {"PERFIS", nullptr, itensPerfis}
    };

    menu.construirMenu(raizItens);
}

void Sistema::aplicarPerfil(const String& nome) {
    DadosPerfil* p = gerenciador.obterPorNome(nome);
    if (!p) return;

    // Atualiza os parâmetros globais de controle
    horaOn = p->horaOn;
    minutoOn = p->minutoOn;
    horaOff = p->horaOff;
    minutoOff = p->minutoOff;
    duracaoRegaMs = p->duracaoRegaMs;
    minutosRega = p->minutosRega;
    perfilAtivo = nome;
    gerenciador.nomeAtivo = nome;

    Serial.println("[Sistema] Perfil aplicado: " + nome);
    logger.registrar("EVENTO,PERFIL," + nome);
}

void Sistema::atualizarCicloLuz() {
    if (!logger.estaAtivo()) return;

    DateTime agora = logger.getRTC().now();
    int minutosAgora = agora.hour() * 60 + agora.minute();
    int minutosOn = horaOn * 60 + minutoOn;
    int minutosOff = horaOff * 60 + minutoOff;

    bool deveEstarLigada = false;
    if (minutosOn < minutosOff) {
        deveEstarLigada = (minutosAgora >= minutosOn && minutosAgora < minutosOff);
    } else {
        deveEstarLigada = (minutosAgora >= minutosOn || minutosAgora < minutosOff);
    }

    if (deveEstarLigada && !luzLigada) {
        luz.ligar();
        luzLigada = true;
        logger.registrar("EVENTO,LUZ,ON");
    } else if (!deveEstarLigada && luzLigada) {
        luz.desligar();
        luzLigada = false;
        logger.registrar("EVENTO,LUZ,OFF");
    }
}

// atualizarCicloRega (similar ao anterior, usando minutosRega e duracaoRegaMs)
// ...

void Sistema::atualizar() {
    // Atualiza as leituras dos sensores
    sensorInterno.atualizar();
    sensorExterno.atualizar();
    sensorLuz.atualizar();

    if (modoHUD) {
        if (btEnter.wasPressed()) {
            modoHUD = false;
            atualizarDisplaySistema();
        } else {
            unsigned long agora = millis();
            if (agora - ultimoDisplay >= 500) {
                ultimoDisplay = agora;
                atualizarHUD();
            }
        }
        return;
    }

    bool buttonPressed = false;
    if (btUp.wasPressed()) {
        menu.navegar(-1);
        buttonPressed = true;
    }
    if (btDown.wasPressed()) {
        menu.navegar(1);
        buttonPressed = true;
    }
    if (btEnter.wasPressed()) {
        menu.selecionar();
        buttonPressed = true;
    }

    atualizarCicloLuz();
    atualizarCicloRega();

    unsigned long agora = millis();
    if (buttonPressed || (agora - ultimoDisplay >= 500)) {
        ultimoDisplay = agora;
        atualizarDisplaySistema();
    }
}

void Sistema::atualizarCicloRega() {
    if (!logger.estaAtivo()) return;

    DateTime agora = logger.getRTC().now();
    int minutoAtual = agora.hour() * 60 + agora.minute();

    // Rega em andamento: verifica se terminou
    if (regaEmAndamento) {
        if (millis() - tempoInicioRega >= duracaoRegaMs) {
            valvulaRega.desligar();
            regaEmAndamento = false;
            logger.registrar("EVENTO,REGA,FIM");
        }
        return;
    }

    // Verifica se o minuto atual coincide com algum horário agendado
    for (int minAgendado : minutosRega) {
        if (minutoAtual == minAgendado && minutoAtual != ultimoMinutoRega) {
            valvulaRega.ligar();
            regaEmAndamento = true;
            tempoInicioRega = millis();
            ultimoMinutoRega = minutoAtual;
            logger.registrar("EVENTO,REGA,INICIO," + String(agora.hour()) + ":" + String(agora.minute()));
            break;
        }
    }

    // Reseta o ultimoMinutoRega na virada do dia
    if (agora.hour() == 0 && agora.minute() == 0) {
        ultimoMinutoRega = -1;
    }
}

void Sistema::atualizarHUD() {
    DadosTela tela;
    tela.titulo = "HUD - SENSORES";

    // 1. Temperatura e Umidade Interna (SHT40)
    String tempInt = "-- C";
    String umidInt = "-- %";
    if (sensorInterno.estaAtivo()) {
        tempInt = String(sensorInterno.getTemperatura(), 1) + "C";
        umidInt = String(sensorInterno.getUmidade(), 0) + "%";
    }
    tela.linhas.push_back({"Int", tempInt + " / " + umidInt});

    // 2. Temperatura e Umidade Externa (SHT40)
    String tempExt = "-- C";
    String umidExt = "-- %";
    if (sensorExterno.estaAtivo()) {
        Serial.println("Sensor externo ativo");
        Serial.println(sensorExterno.getTemperatura());
        tempExt = String(sensorExterno.getTemperatura(), 1) + "C";
        umidExt = String(sensorExterno.getUmidade(), 0) + "%";
    }
    tela.linhas.push_back({"Ext", tempExt + " / " + umidExt});

    // 3. Luminosidade (Lux / PPFD)
    String luzStr = "-- lx";
    if (sensorLuz.estaAtivo()) {
        luzStr = String(sensorLuz.getLux(), 0) + "lx (" + String(sensorLuz.getPPFD(), 0) + "u)";
    }
    tela.linhas.push_back({"Luz", luzStr});

    // 4. Umidade do Solo (4 canais ADC)
    int s1 = analogRead(pinosSolo[0]);
    int s2 = analogRead(pinosSolo[1]);
    int s3 = analogRead(pinosSolo[2]);
    int s4 = analogRead(pinosSolo[3]);
    tela.linhas.push_back({"Solo", String(s1) + " " + String(s2) + " " + String(s3) + " " + String(s4)});

    render.carregar(tela);
    render.desenhar();
}

void Sistema::atualizarDisplaySistema() {
    DadosTela tela;
    String tituloAtual = menu.getTituloAtual();
    int tam = menu.getTamanhoSubmenu();

    if (tituloAtual == "HOME") {
        tela.titulo = perfilAtivo.isEmpty() ? "HOME" : perfilAtivo;

        // Temperatura e Umidade Interna
        String tempInt = "-- C";
        String umidInt = "-- %";
        if (sensorInterno.estaAtivo()) {
            tempInt = String(sensorInterno.getTemperatura(), 1) + " C";
            umidInt = String(sensorInterno.getUmidade(), 0) + "%";
        }

        tela.linhas.push_back({"Luz", luzLigada ? "LIGADA" : "DESLIGADA"});
        tela.linhas.push_back({"Rega", regaEmAndamento ? "REGANDO" : "AGUARDANDO"});
        tela.linhas.push_back({"Temp", tempInt});
        tela.linhas.push_back({"Umid", umidInt});
    } else if (tam > 0) {
        tela.titulo = tituloAtual;
        tela.linhas.push_back({"Selecione", menu.getTituloItemAtual()});
    } else {
        tela.titulo = tituloAtual;
        tela.linhas.push_back({"", ""});
    }

    render.carregar(tela);
    render.desenhar();
}