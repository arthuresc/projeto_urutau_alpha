#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <string>
#include <iostream>
#include <functional>
#include <Display.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_AHTX0.h>
#include <DHT.h>
#include <Fonts/FreeSans9pt7b.h>

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



int vpin = 25;
int val = 0;
int menuState = 0;

Adafruit_AHTX0 aht;

// Define o nível da pressão ao nível do mar (para calcular altitude)
// Esse valor padrão (1013.25 hPa) é uma referência. Você pode ajustar se quiser.
#define SEALEVELPRESSURE_HPA 1010

Adafruit_BME280 bme;

Display telinha;

float temperatura = bme.readTemperature();

String txtTemp = String(bme.readTemperature()) + " C";

String titles[] = {"BME 280", "Pudim de mandioca", "Mouse", "Coxinhaaaa"};
String messages[] = {txtTemp, "meu indigena na sua oca", "é rato em inglês", "de batata"};

PiscaLed led1(2, 100, 400);
PiscaLed led2(4, 100, 400);
void setup() {
  Serial.begin(9600); // Apagar depois
  Serial.println("Sistema iniciando...");
  // pinMode(13, INPUT_PULLUP);
  telinha.init();

  if (!aht.begin())
  {
    Serial.println("Iniciado AHT10");
  }else{
    Serial.println("AHT10 não encontrado");
  }

  if (!bme.begin(0x76)) { // Tenta se comunicar com o endereço I2C mais comum [citation:8]
    Serial.println("Não foi encontrado um sensor BME280 no endereço 0x76. Tentando 0x77...");
    if (!bme.begin(0x77)) { // Se não achou, tenta o outro endereço possível [citation:8]
        Serial.println("Sensor não encontrado. Verifique as conexões!");
        while (1); // Trava o programa aqui
    }
  }

  Serial.println("Sensor BME280 encontrado e inicializado com sucesso!");
  
}

void loop() {
  // led1.Update();
  // led2.Update();
  // Serial.println(vpin);
  val = analogRead(vpin);
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  Serial.println(val);
  // Valores adotados em casa meu pc
  if (val < 2800 && val > 2500) 
  // Valores no note da Iza
  // if (val < 1900 && val > 1500)
  {
    menuState = menuState + 1;
    led1.Update();
  }
  
  if (val > 1200 && val < 1500)
  // if (val > 800 && val < 1000)
  {
    telinha.update((titles[menuState % sizeof(menuState)] + " *"), (messages[menuState % sizeof(menuState)] + " *"));
    delay(1000);
  }
  
  if (val >= 0 && val < 60)
  {
    menuState = menuState - 1;
    led2.Update();
  }
  


  float pressao = bme.readPressure() / 100.0F; // Converte de Pascal para hPa
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float umidade = bme.readHumidity();

  // Mostra os valores no Serial Monitor
  Serial.print("Temperatura = ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Pressão = ");
  Serial.print(pressao);
  Serial.println(" hPa");

  Serial.print("Altitude aprox. = ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Umidade = ");
  Serial.print(umidade);
  Serial.println(" %");

  Serial.println(); // Linha em branco para separar as leituras

  Serial.print("Hum: ");
  Serial.print(humidity.relative_humidity);
  Serial.print(" Temp: ");
  Serial.println(temp.temperature);
  Serial.println();
  telinha.update(titles[menuState % sizeof(menuState)], messages[menuState % sizeof(menuState)]);
  delay(200);
}