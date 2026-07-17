#include "Sistema.h"

Sistema::Sistema()
    : btUp(13), btDown(34), btEnter(15),
      display(), render(display),
      luz(12, false),          // relé lâmpadas (LOW ligado)
      led(2, false),           // relé LED (GPIO 2)
      coolerVentilacao(26, false),
      coolerExaustao(27, false),
      coolerMovimentacao(25, false),
      valvulaRega(14, false),
      sensorInterno(&Wire, 0x44, "SHT_INT"),    // I2C0 (internal SHT40)
      sensorExterno(&Wire1, 0x44, "SHT_EXT"),   // I2C1 (external SHT40)
      sensorLuz(),
      menu(), ultimoDisplay(0),
      config(), gerenciador(config),
      logger(),                // Logger inicializado
      perfilAtivo(""), luzLigada(false), regaEmAndamento(false),
      tempoInicioRega(0), duracaoRegaMs(5000),
      horaOn(6), minutoOn(0), horaOff(24), minutoOff(0),
      ultimoTempoRega(0),
      modoHUD(false),
      ultimoSerialReport(0),
      serialReportIntervalMs(5000),
      lastInitAttemptInterno(0),
      lastInitAttemptExterno(0),
      sensorInitRetryIntervalMs(10000) // tenta re-init a cada 10s se inativo
{
}

void Sistema::iniciar() {
    Serial.begin(9600);
    
    // 1. Inicializar barramentos I2C
    Wire.begin(21, 22);      // I2C0 (display, sensores internos, RTC)
    Wire1.begin(33, 32);     // I2C1 (SDA=33, SCL=32 para SHT40 externo)

    // I2C scan para facilitar debug: lista endereços encontrados em ambos barramentos
    i2cScan(Wire, "I2C0");
    i2cScan(Wire1, "I2C1");

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
        DadosPerfil* primeiroPerfil = gerenciador.obter(0);
        if (primeiroPerfil) {
            perfilAtivo = primeiroPerfil->nome;
            Serial.println("[Sistema] Nenhum perfil ativo definido, usando primeiro: " + perfilAtivo);
        } else {
            Serial.println("[Sistema] ERRO: Nenhum perfil disponível.");
            display.update("ERRO", "Sem perfis");
            while (1) { delay(1000); }
        }
    }

    // 7. Construir menu dinâmico
    construirMenu();

    // 8. Aplicar perfil ativo
    aplicarPerfil(perfilAtivo);

    // 9. Inicializar sensores (opcional, se falhar, não trava)
    sensorInterno.iniciar();
    sensorExterno.iniciar();
    sensorLuz.iniciar();

    // Fazer self-test rápido dos sensores SHT40 para feedback imediato
    sensorInterno.selfTest();
    sensorExterno.selfTest();

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

    // Re-init automático: se sensor marcado inativo, tentar reinicializar periodicamente
    unsigned long nowMs = millis();
    if (!sensorInterno.estaAtivo() && (nowMs - lastInitAttemptInterno >= sensorInitRetryIntervalMs)) {
        Serial.println("[Sistema] Tentando re-inicializar " + sensorInterno.getNome());
        lastInitAttemptInterno = nowMs;
        if (sensorInterno.iniciar()) {
            Serial.println("[Sistema] Re-init OK: " + sensorInterno.getNome());
            sensorInterno.selfTest();
        } else {
            Serial.println("[Sistema] Re-init falhou: " + sensorInterno.getNome());
        }
    }
    if (!sensorExterno.estaAtivo() && (nowMs - lastInitAttemptExterno >= sensorInitRetryIntervalMs)) {
        Serial.println("[Sistema] Tentando re-inicializar " + sensorExterno.getNome());
        lastInitAttemptExterno = nowMs;
        if (sensorExterno.iniciar()) {
            Serial.println("[Sistema] Re-init OK: " + sensorExterno.getNome());
            sensorExterno.selfTest();
        } else {
            Serial.println("[Sistema] Re-init falhou: " + sensorExterno.getNome());
        }
    }

    // Serial input: comandos manuais
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd.length() > 0) {
            if (cmd == "t" || cmd == "T") {
                Serial.println("[Sistema] Comando: selfTest() iniciado para sensores SHT");
                sensorInterno.selfTest();
                sensorExterno.selfTest();
            } else if (cmd == "r" || cmd == "R") {
                // imprimir leituras atuais
                Serial.println("[Sistema] Leituras atuais:");
                Serial.println("  " + sensorInterno.getNome() + " T=" + String(sensorInterno.getTemperatura()) + "C H=" + String(sensorInterno.getUmidade()));
                Serial.println("  " + sensorExterno.getNome() + " T=" + String(sensorExterno.getTemperatura()) + "C H=" + String(sensorExterno.getUmidade()));
            } else {
                Serial.println("[Sistema] Comando desconhecido: " + cmd + " (t=selfTest, r=relatorio)");
            }
        }
    }

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

    // Relatório periódico em Serial das leituras dos sensores (ajustável)
    unsigned long now = millis();
    if (now - ultimoSerialReport >= serialReportIntervalMs) {
        ultimoSerialReport = now;
        Serial.print("[Leituras] ");
        if (sensorInterno.estaAtivo()) {
            Serial.print(sensorInterno.getNome() + ": T=" + String(sensorInterno.getTemperatura(), 1) + "C H=" + String(sensorInterno.getUmidade(), 1) + "%  ");
        } else {
            Serial.print(sensorInterno.getNome() + ": INATIVO  ");
        }
        if (sensorExterno.estaAtivo()) {
            Serial.print(sensorExterno.getNome() + ": T=" + String(sensorExterno.getTemperatura(), 1) + "C H=" + String(sensorExterno.getUmidade(), 1) + "%");
        } else {
            Serial.print(sensorExterno.getNome() + ": INATIVO");
        }
        Serial.println("");
    }
}

void Sistema::atualizarCicloRega() {
    if (!logger.estaAtivo()) return;

    DateTime agora = logger.getRTC().now();
    int minutoAtual = agora.hour() * 60 + agora.minute();
    unsigned long tempoAgora = agora.unixtime();

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
        if (minutoAtual == minAgendado) {
            // Evita disparar novamente no mesmo minuto (com margem de 60 segundos)
            if (tempoAgora - ultimoTempoRega >= 60) {
                valvulaRega.ligar();
                regaEmAndamento = true;
                tempoInicioRega = millis();
                ultimoTempoRega = tempoAgora;
                logger.registrar("EVENTO,REGA,INICIO," + String(agora.hour()) + ":" + String(agora.minute()));
                break;
            }
        }
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

void Sistema::i2cScan(TwoWire& bus, const String& name, bool showOnDisplay) {
    Serial.println("[I2C_SCAN] Scanning bus: " + name);
    bool found = false;
    String addrList = "";
    for (uint8_t addr = 1; addr < 127; ++addr) {
        bus.beginTransmission(addr);
        uint8_t res = bus.endTransmission();
        if (res == 0) {
            char buf[8];
            snprintf(buf, sizeof(buf), "0x%02X", addr);
            Serial.print("  Found device at ");
            Serial.print(buf);
            Serial.print(" (addr=");
            Serial.print(addr);
            Serial.println(")");
            if (addrList.length() > 0) addrList += ",";
            addrList += String(buf);
            found = true;
        }
    }
    if (!found) {
        Serial.println("  Nenhum dispositivo I2C encontrado neste barramento.");
        addrList = "Nenhum";
    }

    // Mostrar resultado resumido no display (se solicitado)
    if (showOnDisplay) {
        // Se a string for muito longa, mostre apenas prefixo
        String shortList = addrList;
        if (shortList.length() > 16) shortList = shortList.substring(0, 16) + "..";
        display.update(name, shortList);
        delay(800);
        // depois volta ao status normal (display será atualizado em seguida)
    }
}
