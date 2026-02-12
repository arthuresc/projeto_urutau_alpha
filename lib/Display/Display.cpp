#include <Display.h>


#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

Display::Display()
  : display(128, 64, &Wire, -1),
    _title("Titulo"),
    _message("Mensagem") {
      // O objeto display é construído na lista de inicialização
      // Não faça inicialização de hardware aqui
      // display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
      // init();
      
    }
    
    
    // 3. Construtor com parâmetros
Display::Display(String title, String message)
  : display(LARGURA_OLED, ALTURA_OLED, &Wire, RESET_OLED),
    _title(title),
    _message(message) {
      // init();
    // this->display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  }

bool Display::initDisplay()
{
  // Inicializa o display SSD1306
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    return false;
  }
  Serial.println("Teste aqui no initDisplay");
  display.clearDisplay();
  // Eu: Acho que esse código está errado
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  return true;
}

// 5. Inicialização pública
  bool Display::init() {
  Serial.println("Iniciou o inicio");
    if(initDisplay()) {
      update(_title, _message);
      Serial.println("Teste aqui no INIT não o outro");
      return true;
    }
    return false;
  }

  void Display::update(String newTitle, String newMessage) {
    // Verifica necessidade de atualização
    // if(newTitle.compareTo(Title) != 0 || newMessage.compareTo(Message) != 0) {
    if(newTitle != _title || newMessage != _message) {
      _title = newTitle;
      _message = newMessage;
      
      Serial.println("Entrou como texto no update");

      display.clearDisplay();

      display.fillRect(0,0,128,16, WHITE);
      display.setTextColor(BLACK);
      display.setCursor(2,4);
      display.print(_title);


      display.setTextColor(WHITE);
      display.setCursor(2, 30);
      display.print(_message);


      // Atualiza a tela
      display.display();
    }
  }

  void Display::clear() {
    display.clearDisplay();
    display.display();

    _title = "";
    _message = "";
  }

  // 8. Verificação de status
bool Display::isInitialized() const {
  // Verifica se o display foi inicializado
  // A biblioteca Adafruit_SSD1306 não tem um método isInitialized,
  // então podemos verificar de outra forma
  return true; // Simplificação - na prática precisa de melhor verificação
}
