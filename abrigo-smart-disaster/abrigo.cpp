#include "sensors.h"
#include "display.h"
#include "alerts.h"
#include "mqtt.h"
#include "api.h"

void configurarAbrigo() {
  configurarDisplay();
  configurarAlertas();
  configurarSensores();
  configurarMqtt();
  configurarApi();

  atualizarDisplay();
  atualizarAlertas();
}

void executarAbrigo() {
  processarSensorEntrada();
  processarBotaoSaida();
  manterMqtt();
  publicarOcupacao();
  processarApi();
}
