#include <Arduino.h>
#include <WebServer.h>
#include "api.h"
#include "config.h"
#include "state.h"
#include "display.h"
#include "alerts.h"

WebServer server(80);

// ─── helpers ────────────────────────────────────────────────────────────────

void addCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

String jsonStatus() {
  String json = "{";
  json += "\"abrigo_id\":"    + String(ABRIGO_ID) + ",";
  json += "\"ocupacao\":"     + String(ocupacaoAtual) + ",";
  json += "\"capacidade\":"   + String(CAPACIDADE_MAXIMA) + ",";
  json += "\"distancia_cm\":" + String(ultimaDistancia, 1) + ",";
  json += "\"status\":\""     + getStatusAbrigo() + "\"";
  json += "}";
  return json;
}

// ─── leitura de body JSON simples ───────────────────────────────────────────
// Extrai valor inteiro de uma chave: {"ocupacao":45} -> 45
int extractInt(String body, String key) {
  int idx = body.indexOf("\"" + key + "\"");
  if (idx < 0) return -1;
  int colon = body.indexOf(":", idx);
  if (colon < 0) return -1;
  return body.substring(colon + 1).toInt();
}

// ─── handlers ────────────────────────────────────────────────────────────────

void handleOptions() {
  addCorsHeaders();
  server.send(204);
}

void handleStatus() {
  addCorsHeaders();
  server.send(200, "application/json", jsonStatus());
}

void handleLeitura() {
  addCorsHeaders();
  String json = "{";
  json += "\"distancia_cm\":" + String(ultimaDistancia, 1) + ",";
  json += "\"timestamp\":\""  + getTimestamp() + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleHistorico() {
  addCorsHeaders();
  String json = "[";
  for (int i = 0; i < historicoTotal; i++) {
    int index = (historicoIndex - historicoTotal + i + 10) % 10;
    json += "{";
    json += "\"ocupacao\":" + String(historico[index].ocupacao) + ",";
    json += "\"ts\":\""     + historico[index].ts + "\"";
    json += "}";
    if (i < historicoTotal - 1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}

// ── POST /controle/entrada  → incrementa ocupação ──────────────────────────
void handleEntrada() {
  addCorsHeaders();
  if (ocupacaoAtual < CAPACIDADE_MAXIMA) {
    ocupacaoAtual++;
    registrarHistorico();
    atualizarDisplay();
    atualizarAlertas();
    server.send(200, "application/json", jsonStatus());
  } else {
    server.send(409, "application/json", "{\"erro\":\"lotado\"}");
  }
}

// ── POST /controle/saida  → decrementa ocupação ────────────────────────────
void handleSaida() {
  addCorsHeaders();
  if (ocupacaoAtual > 0) {
    ocupacaoAtual--;
    registrarHistorico();
    atualizarDisplay();
    atualizarAlertas();
    server.send(200, "application/json", jsonStatus());
  } else {
    server.send(409, "application/json", "{\"erro\":\"zero\"}");
  }
}

// ── POST /controle/definir  body: {"ocupacao":45} → seta valor direto ──────
void handleDefinir() {
  addCorsHeaders();
  String body = server.arg("plain");
  int val = extractInt(body, "ocupacao");
  if (val < 0 || val > CAPACIDADE_MAXIMA) {
    server.send(400, "application/json", "{\"erro\":\"valor invalido\"}");
    return;
  }
  ocupacaoAtual = val;
  registrarHistorico();
  atualizarDisplay();
  atualizarAlertas();
  server.send(200, "application/json", jsonStatus());
}

// ── POST /controle/sensor  body: {"distancia_cm":30.5} → simula sensor ─────
void handleSensor() {
  addCorsHeaders();
  String body = server.arg("plain");
  int idx = body.indexOf("\"distancia_cm\"");
  if (idx >= 0) {
    int colon = body.indexOf(":", idx);
    if (colon >= 0) {
      ultimaDistancia = body.substring(colon + 1).toFloat();
    }
  }
  server.send(200, "application/json", jsonStatus());
}

// ── GET /dashboard  → HTML embutido ────────────────────────────────────────
void handleDashboard() {
  String html = R"HTML(<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>Abrigo IoT — Controle</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Barlow+Condensed:wght@400;700;900&display=swap');
  *{box-sizing:border-box;margin:0;padding:0}
  :root{--ok:#00e87a;--warn:#ffcc00;--crit:#f43f5e;--bg:#05080f;--panel:#080f1c;--edge:#111f36;--muted:#2a4060;--txt:#c8d8f0;--accent:#38bdf8}
  body{background:var(--bg);color:var(--txt);font-family:'Barlow Condensed',sans-serif;min-height:100vh;padding:20px}
  .wrap{max-width:700px;margin:0 auto}
  h1{font-size:1.6rem;font-weight:900;letter-spacing:.1em;color:#fff;text-transform:uppercase;margin-bottom:4px}
  .sub{font-family:'Share Tech Mono',monospace;font-size:.7rem;color:var(--muted);margin-bottom:20px}
  .card{background:var(--panel);border:1px solid var(--edge);border-radius:12px;padding:18px;margin-bottom:14px;position:relative;overflow:hidden}
  .card::before{content:'';position:absolute;top:0;left:0;right:0;height:1px;background:linear-gradient(90deg,transparent,var(--accent),transparent);opacity:.5}
  .label{font-family:'Share Tech Mono',monospace;font-size:.65rem;letter-spacing:.2em;text-transform:uppercase;color:var(--muted);margin-bottom:8px}
  .bignum{font-size:3rem;font-weight:900;color:#fff;text-align:center}
  .prog-wrap{background:#020810;border-radius:8px;height:14px;overflow:hidden;margin:10px 0}
  .prog{height:100%;border-radius:8px;transition:width .5s,background .3s}
  .btn-row{display:flex;gap:10px;justify-content:center;margin-top:12px;flex-wrap:wrap}
  button{font-family:'Barlow Condensed',sans-serif;font-size:1rem;font-weight:700;letter-spacing:.08em;padding:10px 22px;border-radius:8px;cursor:pointer;border:1px solid;transition:all .15s;text-transform:uppercase}
  .btn-in{background:#003d1a;color:var(--ok);border-color:var(--ok)}
  .btn-in:hover{background:#00ff8830}
  .btn-out{background:#1a0008;color:var(--crit);border-color:var(--crit)}
  .btn-out:hover{background:#f43f5e30}
  .btn-set{background:#051525;color:var(--accent);border-color:var(--accent)}
  .btn-set:hover{background:#38bdf820}
  .status{text-align:center;font-size:1.1rem;font-weight:700;letter-spacing:.12em;padding:8px 20px;border-radius:8px;border:1px solid;display:inline-block;margin:8px auto;display:block;width:fit-content;margin:8px auto}
  input[type=range]{width:100%;accent-color:var(--accent);margin:6px 0}
  input[type=number]{background:#020810;border:1px solid var(--edge);border-radius:8px;color:#fff;font-size:1rem;padding:8px 12px;width:100%;font-family:'Barlow Condensed',sans-serif}
  .row2{display:grid;grid-template-columns:1fr 1fr;gap:12px}
  .stat{background:#020810;border:1px solid var(--edge);border-radius:8px;padding:12px;text-align:center}
  .stat-val{font-size:1.8rem;font-weight:900;color:#fff}
  .stat-lbl{font-family:'Share Tech Mono',monospace;font-size:.65rem;color:var(--muted)}
  .log{background:#020810;border-radius:8px;padding:10px;max-height:120px;overflow-y:auto;font-family:'Share Tech Mono',monospace;font-size:.7rem}
  .log div{padding:3px 0;border-bottom:1px solid var(--edge);color:var(--muted)}
  .log div:last-child{border:none}
</style>
</head>
<body>
<div class="wrap">
  <h1>&#x1F6E1; Abrigo IoT — Controle</h1>
  <div class="sub">Abrigo #)HTML" + String(ABRIGO_ID) + R"HTML( &nbsp;|&nbsp; <span id="clk">--:--:--</span></div>

  <div class="row2" style="margin-bottom:14px">
    <div class="stat"><div class="stat-lbl">Ocupação</div><div class="stat-val" id="sOcc">0</div></div>
    <div class="stat"><div class="stat-lbl">Vagas livres</div><div class="stat-val" id="sFree">80</div></div>
  </div>

  <div class="card">
    <div class="label">Ocupação — <span id="occNum">0</span> / )HTML" + String(CAPACIDADE_MAXIMA) + R"HTML(</div>
    <div class="prog-wrap"><div class="prog" id="prog" style="width:0%"></div></div>
    <div id="statusLbl" class="status">DISPONIVEL</div>
    <div class="btn-row">
      <button class="btn-in" onclick="cmd('/controle/entrada','POST')">+ Entrada</button>
      <button class="btn-out" onclick="cmd('/controle/saida','POST')">− Saída</button>
    </div>
  </div>

  <div class="card">
    <div class="label">Definir ocupação manualmente</div>
    <div style="display:flex;gap:8px">
      <input type="number" id="manVal" min="0" max=")HTML" + String(CAPACIDADE_MAXIMA) + R"HTML(" value="0">
      <button class="btn-set" onclick="definir()">Aplicar</button>
    </div>
  </div>

  <div class="card">
    <div class="label">Simular sensor — <span id="distLbl">400</span> cm</div>
    <input type="range" id="distSlider" min="0" max="400" step="1" value="400" oninput="simSensor(this.value)">
    <div style="font-family:'Share Tech Mono',monospace;font-size:.65rem;color:var(--muted);margin-top:4px">Detecta a &lt; 50 cm</div>
  </div>

  <div class="card">
    <div class="label">Log</div>
    <div class="log" id="log"></div>
  </div>
</div>
<script>
const CAP=)HTML" + String(CAPACIDADE_MAXIMA) + R"HTML(;
let distTmr=null;
function ts(){return new Date().toLocaleTimeString('pt-BR');}
function log(msg){const d=document.getElementById('log');const el=document.createElement('div');el.textContent=ts()+' '+msg;d.prepend(el);while(d.children.length>20)d.removeChild(d.lastChild);}
function render(d){
  const occ=d.ocupacao,cap=d.capacidade,pct=Math.min(100,Math.round(occ/cap*100));
  document.getElementById('occNum').textContent=occ;
  document.getElementById('sOcc').textContent=occ;
  document.getElementById('sFree').textContent=Math.max(0,cap-occ);
  const prog=document.getElementById('prog');
  prog.style.width=pct+'%';
  prog.style.background=pct>=100?'#f43f5e':pct>=80?'#ffcc00':'#00e87a';
  const lbl=document.getElementById('statusLbl');
  lbl.textContent=d.status;
  lbl.style.color=pct>=100?'#f43f5e':pct>=80?'#ffcc00':'#00e87a';
  lbl.style.borderColor=lbl.style.color;
  lbl.style.background=pct>=100?'#1a0008':pct>=80?'#1a1200':'#001a0d';
}
async function cmd(path,method,body){
  try{
    const r=await fetch(path,{method,headers:body?{'Content-Type':'application/json'}:{},body:body?JSON.stringify(body):undefined});
    const d=await r.json();
    render(d);
    log(method+' '+path+' OK — ocupação: '+d.ocupacao);
  }catch(e){log('Erro: '+e.message);}
}
function definir(){const v=parseInt(document.getElementById('manVal').value);if(isNaN(v))return;cmd('/controle/definir','POST',{ocupacao:v});}
function simSensor(val){
  document.getElementById('distLbl').textContent=val;
  clearTimeout(distTmr);
  if(parseInt(val)<50){distTmr=setTimeout(()=>{log('Sensor: '+val+'cm → entrada');cmd('/controle/entrada','POST');},600);}
  cmd('/controle/sensor','POST',{distancia_cm:parseFloat(val)});
}
async function poll(){try{const r=await fetch('/status');const d=await r.json();render(d);}catch(_){}}
function tick(){document.getElementById('clk').textContent=new Date().toLocaleTimeString('pt-BR');}
tick();setInterval(tick,1000);
poll();setInterval(poll,4000);
log('Dashboard iniciado');
</script>
</body>
</html>)HTML";

  server.send(200, "text/html", html);
}

// ─── setup ───────────────────────────────────────────────────────────────────

void configurarApi() {
  // leitura
  server.on("/status",             HTTP_GET,     handleStatus);
  server.on("/leitura",            HTTP_POST,    handleLeitura);
  server.on("/historico",          HTTP_GET,     handleHistorico);

  // controle
  server.on("/controle/entrada",   HTTP_POST,    handleEntrada);
  server.on("/controle/saida",     HTTP_POST,    handleSaida);
  server.on("/controle/definir",   HTTP_POST,    handleDefinir);
  server.on("/controle/sensor",    HTTP_POST,    handleSensor);

  // dashboard
  server.on("/dashboard",          HTTP_GET,     handleDashboard);
  server.on("/",                   HTTP_GET,     handleDashboard);

  // CORS pre-flight
  server.on("/status",             HTTP_OPTIONS, handleOptions);
  server.on("/leitura",            HTTP_OPTIONS, handleOptions);
  server.on("/historico",          HTTP_OPTIONS, handleOptions);
  server.on("/controle/entrada",   HTTP_OPTIONS, handleOptions);
  server.on("/controle/saida",     HTTP_OPTIONS, handleOptions);
  server.on("/controle/definir",   HTTP_OPTIONS, handleOptions);
  server.on("/controle/sensor",    HTTP_OPTIONS, handleOptions);

  server.begin();
  Serial.println("WebServer iniciado — porta 80");
  Serial.println("Dashboard: http://localhost:8280/");
  Serial.println("Endpoints de controle:");
  Serial.println("  POST /controle/entrada");
  Serial.println("  POST /controle/saida");
  Serial.println("  POST /controle/definir  {ocupacao: N}");
  Serial.println("  POST /controle/sensor   {distancia_cm: N}");
}

void processarApi() {
  server.handleClient();
}