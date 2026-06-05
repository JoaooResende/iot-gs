 #include <Arduino.h>
#include "alerts.h"
#include "config.h"
#include "state.h"

bool alarmeJaTocou = false;

void configurarAlertas() {
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_VERMELHO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

void tocarAlarmeLotado() {
  for (int i = 0; i < 3; i++) {
    tone(PIN_BUZZER, 1000);
    delay(180);
    noTone(PIN_BUZZER);
    delay(160);
  }
}

void atualizarAlertas() {
  if (ocupacaoAtual >= CAPACIDADE_MAXIMA) {
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_VERMELHO, HIGH);

    if (!alarmeJaTocou) {
      tocarAlarmeLotado();
      alarmeJaTocou = true;
    }
  } else {
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_VERMELHO, LOW);
    noTone(PIN_BUZZER);
    alarmeJaTocou = false;
  }
}
