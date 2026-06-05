#include "state.h"
#include "config.h"

int ocupacaoAtual = 0;
float ultimaDistancia = 999.0;
unsigned long ultimoEnvioMqtt = 0;
unsigned long ultimoEventoSensor = 0;
unsigned long ultimoEventoBotao = 0;

HistoricoItem historico[10];
int historicoIndex = 0;
int historicoTotal = 0;

String getTimestamp() {
  unsigned long segundos = millis() / 1000;
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "2026-06-01T%02lu:%02lu:%02lu",
           (segundos / 3600) % 24,
           (segundos / 60) % 60,
           segundos % 60);
  return String(buffer);
}

String getStatusAbrigo() {
  if (ocupacaoAtual >= CAPACIDADE_MAXIMA) return "LOTADO";
  return "DISPONIVEL";
}

void registrarHistorico() {
  historico[historicoIndex].ocupacao = ocupacaoAtual;
  historico[historicoIndex].ts = getTimestamp();

  historicoIndex = (historicoIndex + 1) % 10;
  if (historicoTotal < 10) historicoTotal++;
}
