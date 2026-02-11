#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>


class Display {
private:
  // 1. Objeto display como membro da classe
  Adafruit_SSD1306 _display;
  
  // Dimensões como constantes (não variáveis)
  static const int LARGURA_OLED = 128;
  static const int ALTURA_OLED = 64;
  static const int RESET_OLED = -1;

  

  String _title;
  String _message;

  bool initDisplay();
  
  public:
  // Construtor - apenas inicializa variáveis
  // A inicialização real do hardware em init()
  Display();
  Display(String title, String message);

  bool init();
  bool init(String title, String message);

  void update(String newTitle, String newMessage);
  void clear();

  String getTitle() const { return _title; }
  String getMessage() const { return _message; }

  bool isInitialized() const;

};
