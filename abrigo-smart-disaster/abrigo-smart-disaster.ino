#include <Arduino.h>
#include "abrigo.h"

void setup() {
  Serial.begin(115200);
  configurarAbrigo();
}

void loop() {
  executarAbrigo();
}
