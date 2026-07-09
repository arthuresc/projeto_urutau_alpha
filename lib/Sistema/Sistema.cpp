#include "Sistema.h"

// ============================================================
// Construtor – valores padrão
// ============================================================
Sistema::Sistema()
: ledInterno(2), btUp(13), btDown(14), btEnter(15),
display(), render(display),
luz(4, true), bombaRega(5, false),
menu(), ultimoDisplay(0),
logger(), config(),
faseAtual(VEGETATIVO), luzLigada(false), regaEmAndamento(false),
horaOn(6), minutoOn(0), horaOff(24), minutoOff(0),
ultimoMinutoRega(-1), tempoInicioRega(0), duracaoRegaMs(5000),
animAtual(nullptr), tempoInicioAnim(0), frameAtualAnim(0), animacaoEmExecucao(false),
modoManual(true)
{ }

// ============================================================
// Inicialização
// ============================================================
void Sistema::iniciar() {
    Serial.begin(9600);
    Serial.println("Sistema Grow Indoor Urutau v1.0");

        // 1. Iniciar os barramentos I2C ANTES de qualquer módulo
    Wire.begin(21, 22);           // I2C0: display, SHT40 interno, BH1750, RTC
    Wire1.begin(33, 32);          // I2C1: SHT40 externo
    
    display.init();
    display.clear();              // limpa qualquer lixo
    delay(100);                   // pequena pausa
    logger.iniciar();      // Inicializa RTC e cartão SD
    
    // Carrega configurações do SD (se existir)
    if (config.carregarDoSD("/config.txt")) {
        horaOn      = config.getInt("HORA_ON", 6);
        minutoOn    = config.getInt("MINUTO_ON", 0);
        horaOff     = config.getInt("HORA_OFF", 24);
        minutoOff   = config.getInt("MINUTO_OFF", 0);
        duracaoRegaMs = (unsigned long)config.getInt("DURACAO_REGA_MS", 5000);
        
        // Horários de rega
        String horariosStr = config.get("HORARIOS_REGA", "08:00,12:00,18:00");
        minutosRega.clear();
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
                minutosRega.push_back(h * 60 + m);
            }
            pos = sep + 1;
        }
    } else {
        Serial.println("[Sistema] Usando configuracoes padrao.");
    }
    
    // Montagem do menu (com callbacks)
    static MenuItem submenuIluminacao[] = {
        {"LIGAR LUZ", [this](){ acaoLigarLuz(); }, {}},
        {"DESLIGAR LUZ", [this](){ acaoDesligarLuz(); }, {}},
        {"VOLTAR", [this](){ menu.voltar(); }, {}}
    };
    static MenuItem submenuRega[] = {
        {"REGAR AGORA", [this](){ acaoRegarAgora(); }, {}},
        {"VOLTAR", [this](){ menu.voltar(); }, {}}
    };
    static MenuItem submenuPerfis[] = {
        {"VEGETACAO", [this](){ acaoPerfilVegetacao(); }, {}},
        {"FLORACAO", [this](){ acaoPerfilFloracao(); }, {}},
        {"GERMINACAO", [this](){ acaoPerfilGerminacao(); }, {}},
        {"MODO MANUAL", [this](){ acaoModoManual(); }, {}},  // novo item
        {"VOLTAR", [this](){ menu.voltar(); }, {}}
    };
    static MenuItem menuPrincipal[] = {
        {"HOME", nullptr, {}},
        {"ILUMINACAO", nullptr, {submenuIluminacao[0], submenuIluminacao[1], submenuIluminacao[2]}},
        {"REGA", nullptr, {submenuRega[0], submenuRega[1]}},
        {"PERFIS", nullptr, {submenuPerfis[0], submenuPerfis[1], submenuPerfis[2],submenuPerfis[3], submenuPerfis[4]}}
    };
    static MenuItem raizContainer = {"MENU PRINCIPAL", nullptr, {
        menuPrincipal[0], menuPrincipal[1], menuPrincipal[2], menuPrincipal[3]
    }};
    menu.setRaiz(raizContainer);
    
    restaurarEstado();     // Tenta carregar último estado do SD
    aplicarPerfil(faseAtual);
    menu.atualizarDisplay();
}

// ============================================================
// Loop principal
// ============================================================
void Sistema::atualizar() {
    // --- Botões ---
    if (btUp.wasPressed()) {
        menu.navegar(-1);
        menu.atualizarDisplay();
    }
    if (btDown.wasPressed()) {
        menu.navegar(1);
        menu.atualizarDisplay();
    }
    if (btEnter.wasPressed()) {
        menu.selecionar();
        menu.atualizarDisplay();
    }
    
    // --- Ciclos automáticos ---
    atualizarCicloLuz();
    atualizarCicloRega();
    atualizarAnimacao();
    
    // --- Display (periódico) ---
    unsigned long agora = millis();
    if (agora - ultimoDisplay >= 500) {
        ultimoDisplay = agora;
        atualizarDisplaySistema();
    }
}

// ============================================================
// Ciclo de luz (com RTC)
// ============================================================
void Sistema::atualizarCicloLuz() {
    if (modoManual) return;
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

// ============================================================
// Ciclo de rega (horários flexíveis, com RTC)
// ============================================================
void Sistema::atualizarCicloRega() {
    if (!logger.estaAtivo()) return;
    if (modoManual) return;
    
    DateTime agora = logger.getRTC().now();
    int minutoAtual = agora.hour() * 60 + agora.minute();
    
    // Rega em andamento: verifica se terminou
    if (regaEmAndamento) {
        if (millis() - tempoInicioRega >= duracaoRegaMs) {
            bombaRega.desligar();
            regaEmAndamento = false;
            logger.registrar("EVENTO,REGA,FIM");
        }
        return;
    }
    
    // Verifica se o minuto atual coincide com algum horário agendado
    for (int minAgendado : minutosRega) {
        if (minutoAtual == minAgendado && minutoAtual != ultimoMinutoRega) {
            bombaRega.ligar();
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

// ============================================================
// Display (HOME e submenus)
// ============================================================
void Sistema::atualizarDisplaySistema() {
    DadosTela tela;
    String tituloAtual = menu.getTituloAtual();
    int tam = menu.getTamanhoSubmenu();
    // if (modoManual) {
    //     tela.linhas.push_back({"Modo", "MANUAL"});
    // } else {
    //     tela.linhas.push_back({"Modo", "AUTO"});
    // }
    
    if (tituloAtual == "HOME") {
        tela.titulo = "HOME";
        tela.linhas.push_back({"Grow Indoor", "Urutau"});
        tela.linhas.push_back({"Luz", luzLigada ? "Ligada" : "Desligada"});
        tela.linhas.push_back({"Rega", regaEmAndamento ? "Regando..." : "Pronta"});
        tela.linhas.push_back({"Temp", "-- C"});
        tela.linhas.push_back({"Umid", "-- %"});
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

// ============================================================
// Animações (placeholder – a serem implementadas com bitmaps)
// ============================================================
void Sistema::iniciarAnimacao(Animacao& anim) {
    if (anim.frames == nullptr || anim.num_frames == 0) return;
    animAtual = &anim;
    frameAtualAnim = 0;
    tempoInicioAnim = millis();
    animacaoEmExecucao = true;
}

void Sistema::atualizarAnimacao() {
    if (!animacaoEmExecucao || animAtual == nullptr) return;
    
    const unsigned long intervalo = 200; // ms entre quadros
    if (millis() - tempoInicioAnim >= intervalo) {
        tempoInicioAnim = millis();
        frameAtualAnim++;
        if (frameAtualAnim >= animAtual->num_frames) {
            frameAtualAnim = 0;
            animacaoEmExecucao = false;  // opcional: parar no fim
        }
        
        // Aqui viria o desenho no display, ex:
        // display.drawBitmap(x, y, animAtual->frames[frameAtualAnim], animAtual->largura, animAtual->altura, 1);
        // display.display();
    }
}

// ============================================================
// Callbacks do menu
// ============================================================
void Sistema::acaoModoManual() {
    modoManual = true;
    Serial.println("Modo Manual ativado. Use os controles manuais de luz e rega.");
    logger.registrar("EVENTO,MODO,MANUAL");
    // Opcional: exibir mensagem no display por alguns segundos
}

void Sistema::acaoLigarLuz() {
    luz.ligar();
    luzLigada = true;
    logger.registrar("EVENTO,LUZ,ON");
}

void Sistema::acaoDesligarLuz() {
    luz.desligar();
    luzLigada = false;
    logger.registrar("EVENTO,LUZ,OFF");
}

void Sistema::acaoRegarAgora() {
    if (!regaEmAndamento) {
        bombaRega.ligar();
        regaEmAndamento = true;
        tempoInicioRega = millis();
        logger.registrar("EVENTO,REGA,INICIO,MANUAL");
    }
}

void Sistema::acaoPerfilVegetacao()  { aplicarPerfil(VEGETATIVO); }
void Sistema::acaoPerfilFloracao()   { aplicarPerfil(FLORACAO); }
void Sistema::acaoPerfilGerminacao() { aplicarPerfil(GERMINACAO); }

// ============================================================
// Aplicação de perfil
// ============================================================
void Sistema::aplicarPerfil(FaseCultivo fase) {
    String nomeSecao;
    switch (fase) {
        case GERMINACAO: nomeSecao = "GERMINACAO"; break;
        case VEGETATIVO: nomeSecao = "VEGETATIVO"; break;
        case FLORACAO:  nomeSecao = "FLORACAO";  break;
    }

    // Carrega da seção, com fallback para os valores padrão que já estavam no código
    horaOn      = config.getIntSecao(nomeSecao, "HORA_ON", 6);
    minutoOn    = config.getIntSecao(nomeSecao, "MINUTO_ON", 0);
    horaOff     = config.getIntSecao(nomeSecao, "HORA_OFF", 24);
    minutoOff   = config.getIntSecao(nomeSecao, "MINUTO_OFF", 0);
    duracaoRegaMs = (unsigned long)config.getIntSecao(nomeSecao, "DURACAO_REGA_MS", 5000);

    // Horários de rega
    String horariosStr = config.getSecao(nomeSecao, "HORARIOS_REGA", "");
    if (horariosStr.length() == 0) {
        // Valores padrão se a chave não estiver no arquivo
        switch (fase) {
            case GERMINACAO: horariosStr = "07:00,09:00,11:00,13:00,15:00,17:00"; break;
            case VEGETATIVO: horariosStr = "08:00,12:00,16:00,20:00"; break;
            case FLORACAO:   horariosStr = "08:00,14:00,20:00"; break;
        }
    }

    // Processa a string de horários (igual ao que você já faz no iniciar)
    minutosRega.clear();
    int pos = 0;
    // while (pos < horariosStr.length()) {
    //     // ... parsing ...
    // }

    // Atualiza o estado do perfil e salva
    faseAtual = fase;
    salvarEstado();
    Serial.println("Perfil aplicado: " + nomeSecao);
}

// ============================================================
// Persistência (salvar/restaurar estado no SD)
// ============================================================
void Sistema::salvarEstado() {
    if (!logger.estaAtivo()) return;
    File f = SD.open("/grow_status.txt", FILE_WRITE);
    if (f) {
        f.printf("perfil=%d\n", (int)faseAtual);
        f.printf("hora_on=%d\n", horaOn);
        f.printf("hora_off=%d\n", horaOff);
        f.close();
    }
}

void Sistema::restaurarEstado() {
    if (!SD.exists("/grow_status.txt")) return;
    File f = SD.open("/grow_status.txt", FILE_READ);
    if (!f) return;
    String linha;
    while (f.available()) {
        linha = f.readStringUntil('\n');
        linha.trim();
        if (linha.startsWith("perfil=")) faseAtual = (FaseCultivo)linha.substring(7).toInt();
        else if (linha.startsWith("hora_on=")) horaOn = linha.substring(8).toInt();
        else if (linha.startsWith("hora_off=")) horaOff = linha.substring(9).toInt();
    }
    f.close();
    Serial.printf("Estado restaurado: perfil %d, luz %02d:00 as %02d:00\n", (int)faseAtual, horaOn, horaOff);
}