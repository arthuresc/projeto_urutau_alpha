#include "CoolerPWM.h"

// Construtor: guarda o pino e o canal de PWM
CoolerPWM::CoolerPWM(uint8_t pino, uint8_t canal)
    : pino(pino), canalPWM(canal), velocidadeAtual(0) {
}

// Inicializa o canal PWM com a frequência fixa de 25kHz e resolução de 8 bits (0-255)
void CoolerPWM::iniciar() {
    ledcSetup(canalPWM, 25000, 8); // 25 kHz, 8 bits
    ledcAttachPin(pino, canalPWM);
    setVelocidade(0);
    Serial.println("[CoolerPWM] Inicializado.");
}

// Define a velocidade (0 a 100%). O PWM é invertido: 100% de velocidade = 0% de duty cycle.
void CoolerPWM::setVelocidade(int porcentagem) {
    porcentagem = constrain(porcentagem, 0, 100);
    velocidadeAtual = porcentagem;
    
    int duty = map(porcentagem, 0, 100, 255, 0); // Mapeamento invertido
    ledcWrite(canalPWM, duty);
}

// Para o cooler completamente
void CoolerPWM::desligar() {
    setVelocidade(0);
}

// Retorna a velocidade atual (útil para o display e logs)
int CoolerPWM::getVelocidade() const {
    return velocidadeAtual;
}