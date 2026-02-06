#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <string>
#include <iostream>
#include <functional>



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

        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        display.setTextColor(BLACK);
        // display.clearDisplay()
        display.setTextSize(1);
        
        
        
        display.clearDisplay();
        display.setCursor(2,4);
        // display.setFont();
        display.fillRect(0, 0, 128, 16, 0x039F);
        display.print(Title);
        display.setTextColor(WHITE);
        display.setCursor(2, 40);
        display.print(Message);
        display.display();
      }
    }
};


class Button {
  int btnPin;
  int8_t btnState;
  unsigned long debounceDelay;
  unsigned long lastDebounceTime = 0;

public: 
  Button(int pin, int debounce = 50)
  {
    btnPin = pin;
    debounceDelay = debounce;

    btnState = 0;
    pinMode(btnPin, INPUT);
  }

  bool wasPressed(){
    
  }
  bool isPressed(){

  }

  void DoSomething(std::function<void()> f_clickAction)
  {
    btnState = !btnState;
    f_clickAction();
  }
};

PiscaLed led1(2, 100, 400);
PiscaLed led2(4, 200, 600);

int buttonState = 1;
int lastButtonState = 4;


const int buttonPin = 17;
unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;

String titles[] = {"Pizaaaaa", "Pudim de mandioca", "Mouse", "Coxinhaaaa"};
String messages[] = {"Eh bao demaaas", "meu indigena na sua oca", "é rato em inglês", "de batata"};


Display telinha("Titulo original", "Mensagem original");

void setup() {
  Serial.begin(9600); // Apagar depois
  pinMode(buttonPin, INPUT);
}

void loop() {
  led1.Update();
  led2.Update();



  int reading = digitalRead(buttonPin);


  Serial.clearWriteError();
  delay(200);
  
  if (reading && (millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();

    if (lastDebounceTime == millis())
    {
      lastDebounceTime = lastDebounceTime + (debounceDelay + 10);
    }
    
    Serial.println("Apertou butao: ");
    Serial.println(buttonState);
    Serial.println(lastButtonState);
    // Serial.println(buttonState % lastButtonState);
    Serial.println("---------____----------");
    if (buttonState != lastButtonState && buttonState % lastButtonState <= 3){
      telinha.Update(titles[buttonState -1], messages[buttonState -1]);
      buttonState = buttonState + 1;
    } else {
      telinha.Update(titles[buttonState -1], messages[buttonState -1]);
      buttonState = 1;
    }
    
  }
  

  
  
  // Primeiro ajusta para não ter loop infinito
  // if (reading != lastButtonState)
  // {
  //   Serial.println("Alterou o lastDebounceTime com millis();");
  //   lastDebounceTime = millis();
  // }
  
  // Serial.println("Loopando o looping!");

  // if((millis() - lastDebounceTime) > debounceDelay){
  //   Serial.println("Passou no primeiro IF");
  //   if(reading != buttonState){
  //     Serial.println("Passou no segundo IF");
  //     buttonState = reading;

  //     buttonRead = buttonRead + 1;
      
  //     Serial.println("Pizza");
  //     if (buttonRead % 2 == 0)
  //     {
  //       Serial.println("Passou no primeiro IF");
  //       telinha.Update("Pizaaaa123", "Eh bao demaaas");
  //     } else if (buttonRead % 2 == 1) {
  //       telinha.Update("Coxinhaaaa", "de batata");
  //     }
  //   }    
  // }
  
  
  // telinha.Update("Pizaaaa123", "Eh bao demaaas");
  // if (buttonState != lastButtonState)
  // {
  //   if (buttonState == HIGH)
  //   {
  //     telinha.Update("Pizaaaa123", "Eh bao demaaas");
  //   }else{

  //     telinha.Update("Coxinhaaaa", "de batata");
  //   }    
  //   // delay(50);
  //   lastButtonState = buttonState;
  //   // delay(3000);
  //   // delay(3000);
  // }
  // telinha.Update("Pizaaaa123", "Eh bao demaaas");
  // delay(3000);

  // telinha.Update("Coxinhaaaa", "de batata");

  
  // delay(3000);  
}