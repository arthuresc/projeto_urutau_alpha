#include <Display.h>

Display::Display()
  : display(LARGURA_OLED, ALTURA_OLED, &Wire, RESET_OLED),
    _title("Titulo"),
    _message("Mensagem"),
    _initialized(false) {
    // O objeto display é construído na lista de inicialização
    // Não faça inicialização de hardware aqui
}
    
    
    // 3. Construtor com parâmetros
Display::Display(String title, String message)
  : display(LARGURA_OLED, ALTURA_OLED, &Wire, RESET_OLED),
    _title(title),
    _message(message),
    _initialized(false) {
}

bool Display::initDisplay()
{
  // Inicializa o display SSD1306
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    return false;
  }
  display.clearDisplay();
  // Eu: Acho que esse código está errado
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  return true;
}

// 5. Inicialização pública
  bool Display::init() {
    _initialized = initDisplay();
    if (_initialized) {
      update(_title, _message);
      Serial.println("Teste aqui no INIT");
    } else {
      Serial.println("Falha ao inicializar display");
    }
    return _initialized;
}

void Display::update(const String& newTitle, const String& newMessage) {
    if (!_initialized) {
      Serial.println("Display não inicializado, pulando update");
      return;
    }
    if (newTitle != _title || newMessage != _message) {
      _title = newTitle;
      _message = newMessage;
      Serial.println("Display: atualizando texto");
      display.clearDisplay();
      display.fillRect(0, 0, 128, 16, WHITE);
      display.setTextColor(BLACK);
      display.setCursor(2, 4);
      display.print(_title);
      display.setTextColor(WHITE);
      display.setCursor(2, 30);
      display.print(_message);
      display.display();
    }
}

  void Display::clear() {
  if (!_initialized) {
    return;
  }
  display.clearDisplay();
  display.display();
  _title = "";
  _message = "";
  }

  bool Display::isInitialized() const {
  return _initialized;
  }
