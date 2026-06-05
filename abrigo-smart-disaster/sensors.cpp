#include <Arduino.h>
#include "sensors.h"
#include "config.h"
#include "state.h"
#include "display.h"
#include "alerts.h"

void configurarSensores() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BOTAO_SAIDA, INPUT_PULLUP);
}

float lerDistanciaCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duracao = pulseIn(PIN_ECHO, HIGH, 30000);
  if (duracao == 0) return 999.0;

  return duracao * 0.034 / 2.0;
}

void processarSensorEntrada() {
  ultimaDistancia = lerDistanciaCm();

  bool detectouPessoa = ultimaDistancia < DISTANCIA_DETECCAO_CM;
  bool passouDebounce = millis() - ultimoEventoSensor > DEBOUNCE_MS;

  if (detectouPessoa && passouDebounce) {
    if (ocupacaoAtual < CAPACIDADE_MAXIMA) {
      ocupacaoAtual++;
      registrarHistorico();
    }

    ultimoEventoSensor = millis();
    atualizarDisplay();
    atualizarAlertas();
  }
}

void processarBotaoSaida() {
  bool botaoPressionado = digitalRead(PIN_BOTAO_SAIDA) == LOW;
  bool passouDebounce = millis() - ultimoEventoBotao > DEBOUNCE_MS;

  if (botaoPressionado && passouDebounce) {
    if (ocupacaoAtual > 0) {
      ocupacaoAtual--;
      registrarHistorico();
    }

    ultimoEventoBotao = millis();
    atualizarDisplay();
    atualizarAlertas();
  }
}
