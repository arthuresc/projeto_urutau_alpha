#include "Sistema.h"

Sistema::Sistema()
    : btUp(13), btDown(34), btEnter(15),
      display(), render(display),
      luz(12, false),          // relé lâmpadas (LOW ligado)
      coolerEntrada(26, false),
      coolerInterno(27, false),
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
    Wire.begin(21, 22);      // I2C0 (display, sensores internos, RTC)
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
    if (modoHUD) {
        atualizarHUD();
        if (btEnter.wasPressed()) modoHUD = false;
        return;
    }

    if (btUp.wasPressed()) { menu.navegar(-1); menu.atualizarDisplay(); }
    if (btDown.wasPressed()) { menu.navegar(1); menu.atualizarDisplay(); }
    if (btEnter.wasPressed()) { menu.selecionar(); menu.atualizarDisplay(); }

    atualizarCicloLuz();
    atualizarCicloRega();

    unsigned long agora = millis();
    if (agora - ultimoDisplay >= 500) {
        ultimoDisplay = agora;
        atualizarDisplaySistema();
    }
}