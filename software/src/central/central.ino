/*
  =====================================================================
  SEMÁFORO SOCIOEMOCIONAL DE AULA — DISPOSITIVO CENTRAL (PIZARRA)
  TEI201 Taller de Diseño en Ingeniería — Avance #2
  Placa: ESP32 DevKit
  =====================================================================
  QUÉ HACE:
   1. SENSOR: lee el nivel de ruido de la sala con el KY-038.
   2. ACTUADOR 1 (RGB #1): muestra el nivel de ruido
        verde = sala tranquila / amarillo = ruido moderado / rojo = ruido alto
   3. CONECTIVIDAD: crea su propia red WiFi ("SemaforoAula").
      A esa red se conectan el botón del escritorio y el celular.
   4. ACTUADOR 2 (RGB #2): se enciende en BLANCO cuando un estudiante
      pide una pausa / ayuda apretando el botón de su escritorio.
   5. Publica una página web en vivo: conectarse a "SemaforoAula" con
      el celular y abrir http://192.168.4.1

  FLUJO: sensor -> procesamiento en ESP32 -> RGB #1
         botón escritorio -> WiFi -> ESP32 central -> RGB #2 + página web

  HISTORIAL (iteración):
   v1: lectura del KY-038 + semáforo en RGB #1, conectado al hotspot
       de un iPhone (dependía de ese celular).
   v2: el ESP32 crea su propia red WiFi, recibe el botón del escritorio,
       enciende la luz blanca y muestra todo en una página web.

  LIBRERÍAS: WiFi.h y WebServer.h (vienen con el paquete ESP32).
  =====================================================================
*/

#include <WiFi.h>
#include <WebServer.h>

// ---------------------- RED WIFI PROPIA ------------------------------
// Antes nos conectábamos al hotspot de un iPhone. Ahora el ESP32 crea
// su propia red, así no dependemos de ningún celular ni del WiFi de la U.
const char* WIFI_NOMBRE = "SemaforoAula";
const char* WIFI_CLAVE  = "aula2026";      // mínimo 8 caracteres

// ---------------------- SENSOR (KY-038) ------------------------------
const int PIN_SENSOR = 34;   // AO del KY-038 (pin ADC1: funciona con WiFi)
const int VENTANA_MS = 50;   // duración de cada medición de amplitud

// Umbrales de amplitud (calibrados por el equipo con el Serial Monitor)
const int UMBRAL_AMARILLO = 17;
const int UMBRAL_ROJO     = 42;

// ---------------------- RGB #1: NIVEL DE RUIDO -----------------------
const int RGB1_R = 25;
const int RGB1_G = 26;
const int RGB1_B = 27;

// ---------------------- RGB #2: LUZ BLANCA (SOLICITUD) ---------------
// Segundo módulo RGB (cátodo común, pata "-" a GND). Se enciende en
// blanco prendiendo R+G+B al mismo tiempo.
const int RGB2_R = 12;
const int RGB2_G = 13;
const int RGB2_B = 14;

// ---------------------- VARIABLES GLOBALES ---------------------------
WebServer servidor(80);

int amplitud = 0;                  // última amplitud medida
String color = "VERDE";            // color actual del semáforo
bool solicitudEstudiante = false;  // true = un estudiante pidió pausa/ayuda
unsigned long horaSolicitud = 0;   // cuándo se hizo la solicitud (ms)

// =====================================================================
// SENSOR Y ACTUADORES
// =====================================================================

// Mide la amplitud del sonido: diferencia entre el valor máximo y
// mínimo que lee el micrófono durante VENTANA_MS milisegundos.
int medirAmplitud() {
  int maximo = 0;
  int minimo = 4095;
  unsigned long inicio = millis();
  while (millis() - inicio < VENTANA_MS) {
    int lectura = analogRead(PIN_SENSOR);
    if (lectura > maximo) maximo = lectura;
    if (lectura < minimo) minimo = lectura;
  }
  return maximo - minimo;
}

// Enciende el RGB #1 según la amplitud medida.
void actualizarSemaforo() {
  digitalWrite(RGB1_R, LOW);
  digitalWrite(RGB1_G, LOW);
  digitalWrite(RGB1_B, LOW);

  if (amplitud <= UMBRAL_AMARILLO) {
    digitalWrite(RGB1_G, HIGH);
    color = "VERDE";
  } else if (amplitud <= UMBRAL_ROJO) {
    digitalWrite(RGB1_R, HIGH);
    digitalWrite(RGB1_G, HIGH);          // rojo + verde = amarillo
    color = "AMARILLO";
  } else {
    digitalWrite(RGB1_R, HIGH);
    color = "ROJO";
  }
}

// Enciende el RGB #2 en blanco (R+G+B) si hay solicitud, o lo apaga.
void actualizarLuzBlanca() {
  int estado = solicitudEstudiante ? HIGH : LOW;
  digitalWrite(RGB2_R, estado);
  digitalWrite(RGB2_G, estado);
  digitalWrite(RGB2_B, estado);
}

// =====================================================================
// PÁGINA WEB Y RUTAS (CONECTIVIDAD)
// =====================================================================

// Página que se ve en el celular: se actualiza sola cada 1 segundo.
const char PAGINA[] PROGMEM = R"HTML(
<!DOCTYPE html><html lang="es"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Semáforo de Aula</title>
<style>
 body{font-family:sans-serif;text-align:center;background:#f4f6f3;margin:0;padding:20px}
 .luz{width:140px;height:140px;border-radius:50%;margin:20px auto;border:6px solid #333;transition:.3s}
 .card{background:#fff;border-radius:14px;padding:16px;margin:14px auto;max-width:360px;box-shadow:0 2px 8px #0002}
 .alerta{background:#fffbe6;border:3px solid #e0b000}
 button{font-size:18px;padding:10px 18px;border-radius:10px;border:none;background:#5a7d63;color:#fff}
</style></head><body>
<h2>Semáforo Socioemocional de Aula</h2>
<div class="card"><div>Nivel de ruido de la sala</div>
 <div id="luz" class="luz"></div>
 <div id="texto" style="font-size:22px;font-weight:bold"></div>
 <div>Amplitud: <span id="nivel">-</span></div></div>
<div id="sol" class="card"><div id="soltexto">Sin solicitudes</div>
 <button id="btn" onclick="fetch('/atender')" style="display:none">Marcar como atendida</button></div>
<script>
const colores={VERDE:'#2ecc40',AMARILLO:'#ffdc00',ROJO:'#ff4136'};
const textos={VERDE:'Sala tranquila',AMARILLO:'Ruido moderado',ROJO:'Ruido alto'};
async function actualizar(){
 try{const r=await fetch('/estado');const e=await r.json();
  document.getElementById('luz').style.background=colores[e.color];
  document.getElementById('texto').textContent=textos[e.color];
  document.getElementById('nivel').textContent=e.amplitud;
  const s=document.getElementById('sol');
  if(e.solicitud){s.classList.add('alerta');
   document.getElementById('soltexto').textContent='Un estudiante pidió una pausa (hace '+e.segundos+' s)';
   document.getElementById('btn').style.display='inline-block';}
  else{s.classList.remove('alerta');
   document.getElementById('soltexto').textContent='Sin solicitudes';
   document.getElementById('btn').style.display='none';}
 }catch(err){}
}
setInterval(actualizar,1000);actualizar();
</script></body></html>
)HTML";

void rutaInicio() {
  servidor.send_P(200, "text/html", PAGINA);
}

// Datos en formato JSON (los usa la página web).
void rutaEstado() {
  unsigned long segundos = solicitudEstudiante ? (millis() - horaSolicitud) / 1000 : 0;
  String json = "{\"color\":\"" + color + "\",\"amplitud\":" + String(amplitud) +
                ",\"solicitud\":" + (solicitudEstudiante ? "true" : "false") +
                ",\"segundos\":" + String(segundos) + "}";
  servidor.send(200, "application/json", json);
}

// La llama el botón del escritorio. Cada presión alterna la solicitud
// (apretar de nuevo = el estudiante la cancela).
void rutaBoton() {
  solicitudEstudiante = !solicitudEstudiante;
  if (solicitudEstudiante) horaSolicitud = millis();
  actualizarLuzBlanca();
  Serial.println(solicitudEstudiante ? ">> WiFi: botón recibido -> LUZ BLANCA ENCENDIDA"
                                     : ">> WiFi: botón recibido -> luz blanca apagada");
  servidor.send(200, "text/plain", solicitudEstudiante ? "ON" : "OFF");
}

// La usa el botón del escritorio para saber si su solicitud sigue activa.
void rutaSolicitud() {
  servidor.send(200, "text/plain", solicitudEstudiante ? "ON" : "OFF");
}

// La usa el profesor desde la página web para apagar la luz blanca.
void rutaAtender() {
  solicitudEstudiante = false;
  actualizarLuzBlanca();
  Serial.println(">> Profesor marcó la solicitud como atendida");
  servidor.send(200, "text/plain", "OK");
}

// =====================================================================
// SETUP Y LOOP
// =====================================================================

void setup() {
  Serial.begin(115200);

  pinMode(RGB1_R, OUTPUT);
  pinMode(RGB1_G, OUTPUT);
  pinMode(RGB1_B, OUTPUT);
  pinMode(RGB2_R, OUTPUT);
  pinMode(RGB2_G, OUTPUT);
  pinMode(RGB2_B, OUTPUT);

  // Prueba de luces al encender: verde, amarillo, rojo y blanco
  digitalWrite(RGB1_G, HIGH);                              delay(300);
  digitalWrite(RGB1_R, HIGH);                              delay(300);
  digitalWrite(RGB1_G, LOW);                               delay(300);
  digitalWrite(RGB1_R, LOW);
  solicitudEstudiante = true;  actualizarLuzBlanca();      delay(300);
  solicitudEstudiante = false; actualizarLuzBlanca();

  // Crear la red WiFi propia
  WiFi.softAP(WIFI_NOMBRE, WIFI_CLAVE);
  Serial.println();
  Serial.print("Red WiFi creada: ");
  Serial.println(WIFI_NOMBRE);
  Serial.print("Pagina web en: http://");
  Serial.println(WiFi.softAPIP());     // normalmente 192.168.4.1

  // Registrar las rutas del servidor web
  servidor.on("/", rutaInicio);
  servidor.on("/estado", rutaEstado);
  servidor.on("/boton", rutaBoton);
  servidor.on("/solicitud", rutaSolicitud);
  servidor.on("/atender", rutaAtender);
  servidor.begin();
}

void loop() {
  servidor.handleClient();             // atender mensajes WiFi

  amplitud = medirAmplitud();          // sensor
  actualizarSemaforo();                // actuador 1

  Serial.print("Amplitud: ");
  Serial.print(amplitud);
  Serial.print(" -> Color: ");
  Serial.print(color);
  Serial.print(" | Luz blanca: ");
  Serial.print(solicitudEstudiante ? "ENCENDIDA" : "apagada");
  Serial.print(" | Conectados: ");
  Serial.println(WiFi.softAPgetStationNum());

  // Espera de 200 ms (como en la v1), atendiendo WiFi mientras tanto
  unsigned long espera = millis();
  while (millis() - espera < 200) {
    servidor.handleClient();
    delay(5);
  }
}
