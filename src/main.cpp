#include <Arduino.h>
#include "Sistema.h"
#include "CoolerPWM.h"
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>

Sistema sistema;

CoolerPWM exaustor(16, 17); // pino 16, canal 0

// #define CS 5 // adicione outros se quiser

// RTC_DS3231 rtc;

void setup() {
  sistema.iniciar();

//   Serial.begin(9600);
//   exaustor.iniciar();
//   Serial.println("=== Teste do Cooler PWM ===");
//   Serial.println("Envie um numero de 0 a 100 para definir a velocidade (%)");
//   Serial.println("Ou envie 'D' para desligar.");
//   Serial.println("===========================");


  // Serial.begin(9600);
  // delay(2000);
  // Serial.println("\n--- Diagnóstico SD ---");

  // // Lista os pinos SPI que serão usados
  // Serial.printf("CS=%d, MOSI=%d, MISO=%d, SCK=%d\n", CS, 23, 19, 18);

  // // Inicializa SPI com velocidade baixa (1 MHz)
  // SPI.begin(18, 19, 23, CS);
  // SPI.setFrequency(400000);

  // // Testa se o cartão é detectado
  // Serial.print("Tentando SD.begin(" + String(CS) + ")... ");
  // if (SD.begin(CS, SPI, 400000)) {
  //   Serial.println("OK!");

  //   uint8_t cardType = SD.cardType();
  //   if (cardType == CARD_NONE) {
  //     Serial.println("Nenhum cartão detectado.");
  //   } else {
  //     Serial.print("Tipo de cartão: ");
  //     if (cardType == CARD_MMC) Serial.println("MMC");
  //     else if (cardType == CARD_SD) Serial.println("SD");
  //     else if (cardType == CARD_SDHC) Serial.println("SDHC");
  //     else Serial.println("Desconhecido");

  //     Serial.print("Tamanho total: ");
  //     Serial.print(SD.totalBytes() / (1024 * 1024));
  //     Serial.println(" MB");
  //   }

  //   // Testa escrita
  //   File f = SD.open("/diag.txt", FILE_WRITE);
  //   if (f) {
  //     f.println("Diagnóstico OK");
  //     f.close();
  //     Serial.println("Arquivo diag.txt criado.");
  //   } else {
  //     Serial.println("Falha ao criar arquivo.");
  //   }
  // } else {
  //   Serial.println("Falhou.");
  //   // Tenta com outros CS comuns
  //   int csAlt[] = {13, 15, 26};
  //   for (int i = 0; i < 3; i++) {
  //     int pin = csAlt[i];
  //     SPI.begin(18, 19, 23, pin); // reconfigura SPI para novo CS
  //     delay(100);
  //     Serial.print("Tentando CS=" + String(pin) + "... ");
  //     if (SD.begin(pin, SPI, 400000)) {
  //       Serial.println("OK! Use este pino CS.");
  //       SD.end();
  //       break;
  //     } else {
  //       Serial.println("Falhou.");
  //     }
  //   }
  // }
}

void loop() {
  sistema.atualizar();

    // if (Serial.available()) {
    //     String entrada = Serial.readStringUntil('\n');
    //     entrada.trim();

    //     if (entrada.equalsIgnoreCase("D")) {
    //         exaustor.desligar();
    //         Serial.println("Cooler desligado (0%).");
    //     } else {
    //         int velocidade = entrada.toInt();
    //         if (velocidade >= 0 && velocidade <= 100) {
    //             exaustor.setVelocidade(velocidade);
    //             Serial.print("Velocidade definida para: ");
    //             Serial.print(velocidade);
    //             Serial.println("%");
    //         } else {
    //             Serial.println("Valor invalido. Digite um numero entre 0 e 100, ou 'D'.");
    //         }
    //     }
    // }
}