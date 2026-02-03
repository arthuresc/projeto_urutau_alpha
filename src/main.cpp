#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <string>

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

PiscaLed led1(2, 100, 400);
PiscaLed led2(4, 200, 600);



class Display {
  // Implementação futura
  String Title;
  String Message;
  int LARGURA_OLED = 128;
  int ALTURA_OLED = 64;
  int RESET_OLED = -1;

  public:
    Display(String title, String message)
    {
      Title = title;
      Message = message;
    }

    void Update(String newTitle, String newMessage)
    {
      if(newTitle.compareTo(Title) != 0 || newMessage.compareTo(Message) != 0) {
        Title = newTitle;
        Message = newMessage;
        Adafruit_SSD1306 display(LARGURA_OLED, ALTURA_OLED, &Wire, RESET_OLED);
        Serial.begin(115200); // Apagar depois

        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        display.setTextColor(BLACK);
        // display.clearDisplay()
        display.setTextSize(1);
        
        
        
        display.clearDisplay();
        display.setCursor(2,2);
        // display.setFont();
        display.fillRect(0, 0, 128, 16, WHITE);
        display.print(Title);
        display.setTextColor(WHITE);
        display.setCursor(2, 40);
        display.print(Message);
        display.display();
      }
    }
};

Display telinha("Titulo original", "Mensagem original");

void setup() {

}

void loop() {
  led1.Update();
  led2.Update();
  telinha.Update("Pizaaaa", "Eh bao demaaas");
  delay(3000);
  telinha.Update("Coxinhaaaa", "de batata");
  delay(3000);  
}