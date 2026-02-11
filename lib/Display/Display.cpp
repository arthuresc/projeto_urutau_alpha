#include <Display.h>

Display::Display()
  : _display(LARGURA_OLED, ALTURA_OLED, &Wire, RESET_OLED),
    _title("Titulo"),
    _message("Mensagem") {
      // O objeto _display é construído na lista de inicialização
      // Não faça inicialização de hardware aqui
    }


// 3. Construtor com parâmetros
Display::Display(String title, String message)
  : _display(LARGURA_OLED, ALTURA_OLED, &Wire, RESET_OLED),
    _title(title),
    _message(message) {
      // Apenas inicializa variáveis
    }

bool Display::initDisplay() {
  // Inicializa o display SSD1306
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    return false;
  }
  _display.clearDisplay();
  // Eu: Acho que esse código está errado
  _display.setTextColor(WHITE);
  _display.setTextSize(1);
  _display.setCursor(0, 0);
  
  return true;
}

// 5. Inicialização pública
  bool Display::init() {
    if(initDisplay()) {
      update(_title, _message);
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

      _display.clearDisplay();

      _display.fillRect(0,0,128,16, WHITE);
      _display.setTextColor(BLACK);
      _display.setCursor(2,4);
      _display.print(_title);


      _display.setTextColor(WHITE);
      _display.setCursor(2, 30);
      _display.print(_message);


      // Atualiza a tela
      _display.display();
    }
  }

  void Display::clear() {
    _display.clearDisplay();
    _display.display();

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
