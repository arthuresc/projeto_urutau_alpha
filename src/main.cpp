#include <Arduino.h>
#include "Button.h"
#include "Led.h"
#include "Display.h"

// ----- Configuração de Hardware -----
#define PIN_LED 2
#define PIN_BOTAO 15

// ----- Componentes -----
Led led(PIN_LED);
Button botao(PIN_BOTAO);
Display display; // sua classe Display (já existente)

// ----- Controle de tempo NÃO BLOQUEANTE -----
unsigned long ultimoPisca = 0;
const unsigned long intervaloPisca = 5500; // pisca a cada 500 ms
bool estadoLed = false;

unsigned long ultimoDisplay = 0;
const unsigned long intervaloDisplay = 1000; // atualiza display a cada 1s

void setup() {
  Serial.begin(9600);
  Serial.println("Sistema Grow Indoor - Loop Não Bloqueante");

  display.init(); // Supondo que sua classe Display tenha um método init()
  display.update("Sistema","Sistema OK");
}

void loop() {
  unsigned long agora = millis();

  // 1. Leitura de botão (SEM delay!)
  // botao.update(); // assumindo que sua classe Button tem update() para debounce
  if (botao.wasPressed()) {
    Serial.println("Botão pressionado!");
    display.update("Sistema", "Botao pressionado");
    // Futuramente, isso vai pro MenuManager
  }

  // 2. Pisca LED sem delay
  if (agora - ultimoPisca >= intervaloPisca) {
    ultimoPisca = agora;
    estadoLed = !estadoLed;
    // String msgEstado = "Entrou aqui no LED" + estadoLed;
    Serial.print("LED");

    if (!estadoLed){ 
      led.toggle();
      display.update("Sistema", "led on");
    }
    else {
      led.toggle();
      display.update("Sistema", "led OFF");
    }
  }

  // 3. Atualiza display periodicamente (sem delay)
  if (agora - ultimoDisplay >= intervaloDisplay) {
    ultimoDisplay = agora;

    String mensagem = "Loop OK " + ultimoPisca;
    // No futuro, aqui vamos ler sensor e atualizar o menu
    display.update("Sistema", mensagem);
  }

  // NENHUM delay() AQUI!
}
