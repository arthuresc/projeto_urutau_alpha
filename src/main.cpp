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



// class Display {
//   // Implementação futura
//   private:
//   String Title;
//   String Message;
//   int LARGURA_OLED = 128;
//   int ALTURA_OLED = 64;
//   int RESET_OLED = -1;
  
//   Adafruit_SSD1306 display;
  
//   public:
//   Display(String title = "Titulo", String message = "Mensagem")
//   : display(LARGURA_OLED, ALTURA_OLED, &Wire, RESET_OLED)
//   {
//     Title = title;
//     Message = message;
    
    
//     // Inicializa o display UMA VEZ no construtor
//     if (!display.begin(SSD1306_SWITCHCAPVCC, 0X3C))
//     {
//       Serial.println("Falha no display OLED");
//       while(1); // Trava o hardware se falhar
//     }
    
//     // Mostra a mensagem inicial
//     Update(Title, Message);
    
//   }
  
//   void Update(String newTitle, String newMessage)
//   {
//     // if(newTitle.compareTo(Title) != 0 || newMessage.compareTo(Message) != 0) {
//     if(newTitle != Title || newMessage != Message) {
//       Title = newTitle;
//       Message = newMessage;
      
      
//       display.clearDisplay();
      
      
      
//       display.setTextColor(BLACK);
//       display.setTextSize(1);
      
      
      
//       display.fillRect(0, 0, 128, 16, WHITE);
//       display.setTextColor(BLACK);
//       display.setCursor(2,4);
//       display.print(Title);
      
      
//       display.setTextColor(WHITE);
//       display.setCursor(2, 30);
//       display.print(Message);
      
//       // Mostra na Tela
//       display.display();
//     }
//   }
  
//   void Clear() {
//     display.clearDisplay();
//     display.display();
//   }
// };


// class Button {
//   private:    
//     byte _pin;
//     unsigned long debounceDelay;
//     unsigned long lastDebounceTime = 0;
//     bool lastState = LOW;
  
  
//   public: 
//     Button(byte pin, int debounce = 50): _pin(pin), debounceDelay(debounce){
//       pinMode(_pin, INPUT); // Mudar para INPUT_PULLUP
//     }
  
//   // void begin() {
//   //   pinMode(_pin, INPUT);
//   // }
  
//   bool isPressed(){
//     return digitalRead(_pin) == HIGH;
//   }
  
//   // bool wasPressed(){
//   //   static bool lastState = LOW;
//   //   bool currentState = isPressed();
//   //   bool pressed = (currentState == HIGH && lastState == LOW);
//   //   bool debounced = (pressed && (millis() - lastDebounceTime) > debounceDelay);
//   //   if (debounced)
//   //   {
//   //     delay(21);
//   //     lastDebounceTime = millis();
//   //     return true;
//   //   }else{
//   //     return false;
//   //   }
  
//   // }
  
//   bool wasPressed() {
//     bool currentState = isPressed();
//     bool pressed = true;
    
//     if (currentState != lastState) 
//     {
//       if ((millis() - lastDebounceTime) > debounceDelay)
//       {
//         pressed = currentState;
//         lastDebounceTime = millis();
//       }
//       lastState = currentState;
//     }
//     return pressed;    
//   }
  
//   bool wasClicked() {
//     static bool waitingForRelease = false;
//     static unsigned long pressTime = 0;
    
//     if(isPressed() && !waitingForRelease) {
//       waitingForRelease = true;
//       pressTime = millis();
//       return false;
//     }
    
//     if (!isPressed() && waitingForRelease) {
//       waitingForRelease = false;
//       return (millis() - pressTime) < 500;
//     }
    
//     return false;    
//   }
// };


int menuState = 0;
int menuSize = 4;

String titles[] = {"Pizaaaaa", "Pudim de mandioca", "Mouse", "Coxinhaaaa"};
String messages[] = {"Eh bao demaaas", "meu indigena na sua oca", "é rato em inglês", "de batata"};


// Display telinha("Titulo original", "Mensagem original");

// Button btn(17);


int vpin = analogRead(17);

PiscaLed led1(2, 100, 400);
PiscaLed led2(4, 200, 600);
void setup() {
  Serial.begin(9600); // Apagar depois
  Serial.println("Sistema iniciando...");
  // pinMode(13, INPUT_PULLUP);
}

void loop() {
  led1.Update();
  led2.Update();

  
}