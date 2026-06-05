#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

struct HistoricoItem {
  int ocupacao;
  String ts;
};

extern int ocupacaoAtual;
extern float ultimaDistancia;
extern unsigned long ultimoEnvioMqtt;
extern unsigned long ultimoEventoSensor;
extern unsigned long ultimoEventoBotao;

extern HistoricoItem historico[10];
extern int historicoIndex;
extern int historicoTotal;

String getTimestamp();
String getStatusAbrigo();
void registrarHistorico();

#endif
