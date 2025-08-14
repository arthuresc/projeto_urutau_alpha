#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SDA_PIN 13
#define SCL_PIN 14
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUTTON_PIN 4
#define LED_PIN 2


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {

  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display não encontrado!");
    while(1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Pinos Customizados!");
  display.print("SDA: GPIO");
  display.println(SDA_PIN);
  display.print("SCL: GPIO");
  display.println(SCL_PIN);
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:

  int buttonState = digitalRead(BUTTON_PIN);

  if(buttonState == LOW) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

    delay(500);
  } else {

    digitalWrite(LED_PIN, LOW);
  }

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}