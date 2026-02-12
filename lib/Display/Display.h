#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>


class Display {
private:
  // 1. Objeto display como membro da classe
  // Use a pointer and construct the Adafruit_SSD1306 instance in the .cpp to avoid the "most vexing parse"

  Adafruit_SSD1306 display; 
  
  static const int LARGURA_OLED = 128;
  static const int ALTURA_OLED = 64;
  static const int RESET_OLED = -1;
  
  // Dimensões como constantes (não variáveis)

  

  String _title;
  String _message;

  bool initDisplay();
  // void printTitle();
  // void printMessage();
  
  public:
  // Construtor - apenas inicializa variáveis
  // A inicialização real do hardware em init()
  Display();
  Display(String title, String message);

  bool init();
  // bool init(String title, String message);

  void update(String newTitle, String newMessage);
  void clear();

  String getTitle() const { return _title; }
  String getMessage() const { return _message; }

  bool isInitialized() const;

};
