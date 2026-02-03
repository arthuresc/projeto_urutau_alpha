#include "Display.h"

// Instância global do display para uso em todas as funções deste arquivo
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Display::Display() {
  // Inicializa os parâmetros do display
  SCREEN_WIDTH = 128;
  SCREEN_HEIGHT = 64;
  OLED_RESET = -1;       // Pino de reset (-1 se não usado)
  I2C_ADDRESS = 0x3C;    // Endereço I2C padrão para muitos displays 
}
// Construtor vazio
void Button::begin() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    Serial.println("Erro no OLED!");
    while(1);
  }
}




  // Função para mostrar texto centralizado
  void textoCentralizado(String texto, int tamanhoTexto) {
    begin();
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
