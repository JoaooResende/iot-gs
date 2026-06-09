# 🛡️ Smart Disaster — Abrigo IoT

LINK DO VIDEO https://youtu.be/CWlAwDi8Oac
VIDEO PITCH SOBRE O PROJETO : https://www.youtube.com/watch?v=ItPEbWxzNkw

Sistema de monitoramento de ocupação de abrigos em situações de desastre.  
ESP32 + sensor ultrassônico + MQTT + Web Server + Dashboard HTML moderno.

---

## ⚡ Como rodar no Wokwi (VS Code)

### Pré-requisitos
- VS Code com extensão **Wokwi for VS Code** instalada
- Extensão **Arduino** ou **PlatformIO** para compilar

### Passo a passo

1. **Abra a pasta** `abrigo-smart-disaster/` no VS Code
2. **Compile** o sketch via Arduino IDE ou PlatformIO  
   - Board: `ESP32 Dev Module`
   - O arquivo `.bin` gerado vai para `build/esp32.esp32.esp32doit-devkit-v1/`
3. **Inicie a simulação** no Wokwi (clique no `diagram.json`)
4. **Acesse o dashboard embutido** em: `http://localhost:8280/`

> O `wokwi.toml` já configura o port-forward `localhost:8280 → ESP32:80`

---

## 🌐 Dashboard Externo

Abra `docs/index.html` direto no navegador.  
Ele se conecta via:
- **MQTT WebSocket** → `wss://broker.hivemq.com:8884/mqtt`  
- **REST polling** → `http://localhost:8280` (quando Wokwi está rodando)

---

## 📡 Endpoints REST (ESP32 WebServer)

| Endpoint     | Método | Descrição                          |
|-------------|--------|------------------------------------|
| `/`         | GET    | Dashboard HTML embutido            |
| `/dashboard`| GET    | Dashboard HTML embutido            |
| `/status`   | GET    | JSON com ocupação e status atual   |
| `/leitura`  | POST   | JSON com última leitura do sensor  |
| `/historico`| GET    | Array JSON com últimas 10 leituras |

### Exemplo `/status`
```json
{ "abrigo_id": 1, "ocupacao": 45, "capacidade": 80, "distancia_cm": 12.4, "status": "DISPONIVEL" }
```

---

## 🔌 Pinagem ESP32

| Componente   | Pino ESP32 |
|-------------|-----------|
| HC-SR04 TRIG | GPIO 5   |
| HC-SR04 ECHO | GPIO 18  |
| Botão Saída  | GPIO 33  |
| LED Verde    | GPIO 26  |
| LED Vermelho | GPIO 27  |
| Buzzer       | GPIO 32  |
| LCD SDA      | GPIO 21  |
| LCD SCL      | GPIO 22  |

---

## 📦 Bibliotecas Arduino

- `LiquidCrystal I2C` (Marcos Schwartz)
- `PubSubClient` (Nick O'Leary)
- `WebServer` (built-in ESP32 Arduino core)
- `WiFi` (built-in ESP32 Arduino core)

---

## 🗂️ Estrutura do Projeto

```
abrigo-smart-disaster/
├── abrigo-smart-disaster.ino  ← ponto de entrada (setup/loop)
├── abrigo.h / abrigo.cpp      ← orquestra todos os módulos
├── config.h                   ← pinos, SSID, MQTT, constantes
├── state.h / state.cpp        ← variáveis globais compartilhadas
├── sensors.h / sensors.cpp    ← HC-SR04 + botão
├── display.h / display.cpp    ← LCD I2C
├── alerts.h / alerts.cpp      ← LEDs + buzzer
├── mqtt.h / mqtt.cpp          ← WiFi + PubSubClient
├── api.h / api.cpp            ← WebServer + endpoints + dashboard
├── diagram.json               ← circuito Wokwi
├── wokwi.toml                 ← config simulador + port-forward
└── wokwi-libraries.txt        ← libs do Wokwi
docs/
└── index.html                 ← dashboard externo (MQTT + REST)
```
