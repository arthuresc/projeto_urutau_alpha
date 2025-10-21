#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1       // Pino de reset (-1 se não usado)
#define I2C_ADDRESS 0x3C   /

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Função para mostrar texto centralizado
void textoCentralizado(String texto, int tamanhoTexto) {
  int16_t x1, y1;
  uint16_t largura, altura;

  // Configura o tamanho do texto
  display.setTextSize(tamanhoTexto);
  
  // Obtém os limites do texto (a string, a posição inicial arbitrária (0,0), e retorna x1, y1, largura, altura)
  display.getTextBounds(texto, 0, 0, &x1, &y1, &largura, &altura);

  // Calcula a posição X e Y para centralizar o texto
  int x = (SCREEN_WIDTH - largura) / 2;
  int y = (SCREEN_HEIGHT - altura) / 2;

  // Limpa o display e define o cursor para a posição calculada
  display.clearDisplay();
  display.setCursor(x, y);
  display.println(texto);
  display.display();
}
void txtTitle(String texto, int tamanhoTexto) {
  int16_t x1, y1;
  uint16_t largura, altura;

  // Configura o tamanho do texto
  display.setTextSize(tamanhoTexto);
  
  // Obtém os limites do texto (a string, a posição inicial arbitrária (0,0), e retorna x1, y1, largura, altura)
  display.getTextBounds(texto, 0, 0, &x1, &y1, &largura, &altura);

  // Calcula a posição X e Y para centralizar o texto
  int x = (SCREEN_WIDTH - largura) / 2;
  int y = 0;

  // Limpa o display e define o cursor para a posição calculada
  display.clearDisplay();
  display.setCursor(x, y);
  display.println(texto);
  display.display();
}

void textoAlinhadoEsquerda(String texto, int tamanhoTexto) {
  int16_t x1, y1;
  uint16_t largura, altura;

  // Configura o tamanho do texto
  display.setTextSize(tamanhoTexto);
  
  // Obtém os limites do texto (a string, a posição inicial arbitrária (0,0), e retorna x1, y1, largura, altura)
  display.getTextBounds(texto, 0, 0, &x1, &y1, &largura, &altura);

  // Calcula a posição X e Y para centralizar o texto
  int x = SCREEN_WIDTH / 2;
  int y = SCREEN_HEIGHT / 2;

  // Limpa o display e define o cursor para a posição calculada
  display.clearDisplay();
  display.setCursor(x, y);
  display.println(texto);
  display.display();
}