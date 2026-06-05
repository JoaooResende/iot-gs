#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi Wokwi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// MQTT
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "smartdisaster/abrigo/001/ocupacao"

// Abrigo
#define ABRIGO_ID 1
#define CAPACIDADE_MAXIMA 80

// Pinos
#define PIN_TRIG 5
#define PIN_ECHO 18
#define PIN_BOTAO_SAIDA 33
#define PIN_LED_VERDE 26
#define PIN_LED_VERMELHO 27
#define PIN_BUZZER 32

// LCD I2C
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Regras
#define DISTANCIA_DETECCAO_CM 50
#define INTERVALO_MQTT 5000
#define DEBOUNCE_MS 350

#endif
