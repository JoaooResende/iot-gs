#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "mqtt.h"
#include "config.h"
#include "state.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void conectarWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void conectarMqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando MQTT...");

    String clientId = "abrigo-smart-disaster-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println(" conectado");
    } else {
      Serial.print(" erro=");
      Serial.println(mqttClient.state());
      delay(1200);
    }
  }
}

void configurarMqtt() {
  conectarWifi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  conectarMqtt();
}

void manterMqtt() {
  if (!mqttClient.connected()) conectarMqtt();
  mqttClient.loop();
}

void publicarOcupacao() {
  if (millis() - ultimoEnvioMqtt < INTERVALO_MQTT) return;

  String payload = "{";
  payload += "\"abrigo_id\":" + String(ABRIGO_ID) + ",";
  payload += "\"ocupacao\":" + String(ocupacaoAtual) + ",";
  payload += "\"capacidade\":" + String(CAPACIDADE_MAXIMA) + ",";
  payload += "\"status\":\"" + getStatusAbrigo() + "\",";
  payload += "\"ts\":\"" + getTimestamp() + "\"";
  payload += "}";

  mqttClient.publish(MQTT_TOPIC, payload.c_str());
  ultimoEnvioMqtt = millis();

  Serial.print("MQTT -> ");
  Serial.println(payload);
}
