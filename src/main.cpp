#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <string>
#include <iostream>
#include <functional>
#include <Display.h>



class PiscaLed {
  int ledPin; // numero do pino do LED
  long OnTime; // ms do tempo ligado
  long OffTime; // ms do tempo desligado
  
  int ledState; // ledState usada para guardar o estado do LED
  unsigned long previousMillis; // vai guardar o ultimo acionamento do LED
  
  
  public:
  PiscaLed(int pin, long on, long off)
  {
    ledPin = pin;
    
    OnTime = on;
    OffTime = off;
    
    ledState = LOW;
    previousMillis = 0;
    pinMode(ledPin, OUTPUT);
  }
  
  void Update()
  {
    // Faz a checagem para saber se já é o momento de alterar o estado do LED
    unsigned long currentMillis = millis();
    
    
    
    if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime)) 
    {
      ledState = LOW; // Desliga o LED
      previousMillis = currentMillis; // Guarda o tempo
      digitalWrite(ledPin, ledState); // Faz o update do LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH; // Liga o LED
      previousMillis = currentMillis; // Guarda o tempo
      digitalWrite(ledPin, ledState);
    }
    
    
  }
};


String titles[] = {"Pizaaaaa", "Pudim de mandioca", "Mouse", "Coxinhaaaa"};
String messages[] = {"Eh bao demaaas", "meu indigena na sua oca", "é rato em inglês", "de batata"};



// Button btn(17);


// int vpin = analogRead(17);
Display telinha;


PiscaLed led1(2, 100, 400);
PiscaLed led2(4, 200, 600);
void setup() {
  Serial.begin(9600); // Apagar depois
  Serial.println("Sistema iniciando...");
  // pinMode(13, INPUT_PULLUP);
  telinha.init();
}

void loop() {
  led1.Update();
  led2.Update();
  telinha.update("Piranhaaaa", "Eh um bixo que vive no rio amazonas");
}