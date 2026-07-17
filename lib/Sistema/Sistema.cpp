#include "Sistema.h"
#include <WiFi.h>
#include <time.h>

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
      sensorInitRetryIntervalMs(10000), // tenta re-init a cada 10s se inativo
      retryAttemptsInterno(0),
      retryAttemptsExterno(0),
      baseRetryIntervalMs(10000),
      maxRetryIntervalMs(600000),
      prevSensorInternoAtivo(false),
      prevSensorExternoAtivo(false)
{
}

void Sistema::iniciar() {
    Serial.begin(9600);
    
    // 1. Inicializar barramentos I2C
    Wire.begin(21, 22);      // I2C0 (display, sensores internos, RTC)
    Wire1.begin(33, 32);     // I2C1 (SDA=33, SCL=32 para SHT40 externo)

    // I2C scan para facilitar debug: lista endereços encontrados em ambos barramentos
    i2cScan(Wire, "I2C0", true);
    i2cScan(Wire1, "I2C1", true);

    // 2. Display
    display.init();
    delay(100);

    // 3. Logger (RTC + SD)
    logger.iniciar();

    // 3.1 Tentar sincronizar hora via NTP se credenciais WiFi estiverem em config.txt
    trySyncTimeWithNTP();

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
    sensorInterno.iniciar(3, 200); // 3 tentativas com 200ms entre tentativas
    sensorExterno.iniciar(3, 200);
    sensorLuz.iniciar();

    // Fazer self-test rápido dos sensores SHT40 para feedback imediato
    sensorInterno.selfTest();
    sensorExterno.selfTest();

    // Inicializa flags de estado para detecção de transições
    prevSensorInternoAtivo = sensorInterno.estaAtivo();
    prevSensorExternoAtivo = sensorExterno.estaAtivo();

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

    // Re-init automático com backoff exponencial: se sensor marcado inativo, tentar reinicializar periodicamente
    unsigned long nowMs = millis();

    // Interno
    if (!sensorInterno.estaAtivo()) {
        uint8_t capAttempts = retryAttemptsInterno > 6 ? 6 : retryAttemptsInterno; // evita shift overflow
        unsigned long factor = (1UL << capAttempts);
        unsigned long waitMs = baseRetryIntervalMs * factor;
        if (waitMs > maxRetryIntervalMs) waitMs = maxRetryIntervalMs;
        if (nowMs - lastInitAttemptInterno >= waitMs) {
            Serial.println("[Sistema] Tentando re-inicializar " + sensorInterno.getNome() + " (attempt=" + String(retryAttemptsInterno) + ", wait=" + String(waitMs) + "ms)");
            lastInitAttemptInterno = nowMs;
            bool ok = sensorInterno.iniciar();
            if (ok) {
                Serial.println("[Sistema] Re-init OK: " + sensorInterno.getNome());
                retryAttemptsInterno = 0;
                sensorInterno.selfTest();
                logger.registrar("EVENTO,SENSOR," + sensorInterno.getNome() + ",RECUPERADO");
            } else {
                retryAttemptsInterno++;
                Serial.println("[Sistema] Re-init falhou: " + sensorInterno.getNome());
                logger.registrar("EVENTO,SENSOR," + sensorInterno.getNome() + ",FALHA_REINIT");
            }
        }
    }

    // Externo
    if (!sensorExterno.estaAtivo()) {
        uint8_t capAttemptsE = retryAttemptsExterno > 6 ? 6 : retryAttemptsExterno;
        unsigned long factorE = (1UL << capAttemptsE);
        unsigned long waitMsE = baseRetryIntervalMs * factorE;
        if (waitMsE > maxRetryIntervalMs) waitMsE = maxRetryIntervalMs;
        if (nowMs - lastInitAttemptExterno >= waitMsE) {
            Serial.println("[Sistema] Tentando re-inicializar " + sensorExterno.getNome() + " (attempt=" + String(retryAttemptsExterno) + ", wait=" + String(waitMsE) + "ms)");
            lastInitAttemptExterno = nowMs;
            bool ok = sensorExterno.iniciar();
            if (ok) {
                Serial.println("[Sistema] Re-init OK: " + sensorExterno.getNome());
                retryAttemptsExterno = 0;
                sensorExterno.selfTest();
                logger.registrar("EVENTO,SENSOR," + sensorExterno.getNome() + ",RECUPERADO");
            } else {
                retryAttemptsExterno++;
                Serial.println("[Sistema] Re-init falhou: " + sensorExterno.getNome());
                logger.registrar("EVENTO,SENSOR," + sensorExterno.getNome() + ",FALHA_REINIT");
            }
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

    // Detectar transições de estado dos sensores e logar eventos
    if (prevSensorInternoAtivo && !sensorInterno.estaAtivo()) {
        Serial.println("[Sistema] Sensor interno ficou INATIVO: " + sensorInterno.getNome());
        logger.registrar("EVENTO,SENSOR," + sensorInterno.getNome() + ",INATIVO");
        prevSensorInternoAtivo = false;
    } else if (!prevSensorInternoAtivo && sensorInterno.estaAtivo()) {
        Serial.println("[Sistema] Sensor interno RECUPERADO: " + sensorInterno.getNome());
        logger.registrar("EVENTO,SENSOR," + sensorInterno.getNome() + ",ATIVO");
        prevSensorInternoAtivo = true;
    }

    if (prevSensorExternoAtivo && !sensorExterno.estaAtivo()) {
        Serial.println("[Sistema] Sensor externo ficou INATIVO: " + sensorExterno.getNome());
        logger.registrar("EVENTO,SENSOR," + sensorExterno.getNome() + ",INATIVO");
        prevSensorExternoAtivo = false;
    } else if (!prevSensorExternoAtivo && sensorExterno.estaAtivo()) {
        Serial.println("[Sistema] Sensor externo RECUPERADO: " + sensorExterno.getNome());
        logger.registrar("EVENTO,SENSOR," + sensorExterno.getNome() + ",ATIVO");
        prevSensorExternoAtivo = true;
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

void Sistema::trySyncTimeWithNTP() {
    // Lê credenciais do config (se disponíveis)
    String ssid = config.get("WIFI_SSID", "");
    String pass = config.get("WIFI_PASS", "");
    if (ssid.isEmpty()) {
        Serial.println("[NTP] WIFI_SSID não configurado, pulando sincronização NTP.");
        return;
    }

    Serial.println("[NTP] Conectando à rede WiFi: " + ssid);
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long start = millis();
    const unsigned long timeoutMs = 15000; // 15s
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
        delay(200);
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[NTP] Falha ao conectar WiFi, pulando NTP.");
        return;
    }

    String ntp = config.get("NTP_SERVER", "pool.ntp.org");
    long gmtOffsetSec = config.getInt("GMT_OFFSET_SEC", 0);
    int daylightOffsetSec = config.getInt("DAYLIGHT_OFFSET_SEC", 0);
    Serial.println("[NTP] ConfigTime -> server=" + ntp + " GMToffset=" + String(gmtOffsetSec));
    configTime(gmtOffsetSec, daylightOffsetSec, ntp.c_str());

    // aguarda tempo válido
    time_t now = time(nullptr);
    unsigned long waitStart = millis();
    while (now < 1600000000 && millis() - waitStart < 10000) { // 10s
        delay(200);
        now = time(nullptr);
    }

    if (now < 1600000000) {
        Serial.println("[NTP] Não foi possível obter hora via NTP.");
    } else {
        Serial.println("[NTP] Hora obtida via NTP: " + String((unsigned long)now));
        // Atualiza RTC se o logger/RTC estiver disponível
        if (logger.estaAtivo()) {
            logger.getRTC().adjust(DateTime((uint32_t)now));
            Serial.println("[NTP] RTC atualizado com hora NTP.");
            logger.registrar("EVENTO,NTP,SYNC");
        } else {
            Serial.println("[NTP] Logger/RTC inativo; hora NTP não aplicada ao RTC.");
        }
    }

    // Desconectar WiFi para economizar energia (opcional)
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
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
