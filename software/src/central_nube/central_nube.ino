// central_nube.ino — Semáforo de ruido de la sala conectado a Railway
// Base: código final del grupo (KY-038 + semáforo + aviso por WiFi).
// Agregado (guía Railway, sección 12): el ESP32 manda los datos al servidor.
//   - cada 10 s el promedio del ruido y su color -> POST /api/lecturas
//   - cuando llega un aviso del botón             -> POST /api/asistencia
//   - mientras el aviso está activo pregunta       -> GET  /api/estado
//     El LED de aviso queda encendido hasta que la profesora entra a la
//     página con su clave y lo marca como atendido.
// Cambio (23-11-2026): el LED del semáforo ya no cambia con cada medición
//   de 50 ms. Muestra el color del PROMEDIO de los últimos 10 s, el mismo
//   valor que se manda a la página. Así la luz cambia como máximo una vez
//   cada 10 s y no distrae a los niños.
// Librerías: WiFi, WebServer, HTTPClient (vienen con el core ESP32),
//            ArduinoJson versión 7 (Library Manager).

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ---------- CAMBIAR ESTOS DATOS (como en la guía) ----------
const char* WIFI_SSID   = "NOMBRE_DEL_HOTSPOT";    // red CON internet, 2.4 GHz
const char* WIFI_PASS   = "CLAVE_DEL_HOTSPOT";
const char* SERVER_URL  = "https://semaforoenlinea.up.railway.app";   // sin "/" al final
const char* API_KEY     = "la-misma-clave-que-pusiste-en-railway";
const char* DISPOSITIVO = "semaforo-sala";
// -----------------------------------------------------------

const unsigned long INTERVALO_MS = 10000;   // cada 10 s: promedio, color del LED y envío

// ---------- Código del grupo (sin cambios) ----------
const int PIN_SENSOR = 34;
const int VENTANA_MS = 50;
const int RGB1_R = 26;
const int RGB1_G = 27;
const int RGB1_B = 25;
const int LED_AVISO = 13;

const int UMBRAL_AMARILLO = 25;
const int UMBRAL_ROJO     = 50;
const unsigned long CONSULTA_MS = 3000;    // cada cuánto pregunta si ya lo atendieron

WebServer server(80);
volatile bool avisoActivo = false;       // LED de aviso encendido

// ---------- Variables para la nube ----------
volatile bool avisoPendiente = false;   // falta avisar al servidor
volatile bool avisoEnServidor = false;  // el servidor ya lo tiene registrado
portMUX_TYPE candado = portMUX_INITIALIZER_UNLOCKED;

// ---------- Promedio de 10 s (lo usan el LED y la página) ----------
long sumaAmplitud = 0;                   // suma de las mediciones de 50 ms
long cuentaAmplitud = 0;                 // cuántas mediciones lleva
unsigned long inicioPromedio = 0;        // cuándo empezó el promedio actual
int promedioParaEnviar = 0;              // último promedio, para la nube
bool hayPromedioNuevo = false;           // la nube todavía no lo manda

const char* nivelDe(int amplitud) {      // como lo pide el servidor
  if (amplitud <= UMBRAL_AMARILLO) return "verde";
  if (amplitud <= UMBRAL_ROJO) return "amarillo";
  return "rojo";
}

// Enciende el LED del semáforo con el color que corresponde al ruido
void mostrarColor(int amplitud) {
  digitalWrite(RGB1_R, LOW);
  digitalWrite(RGB1_G, LOW);
  digitalWrite(RGB1_B, LOW);

  if (amplitud <= UMBRAL_AMARILLO) {
    digitalWrite(RGB1_G, HIGH);
  } else if (amplitud <= UMBRAL_ROJO) {
    digitalWrite(RGB1_R, HIGH);
    digitalWrite(RGB1_G, HIGH);
  } else {
    digitalWrite(RGB1_R, HIGH);
  }
}

void atenderAviso() {
  if (!avisoActivo) {                    // si ya estaba encendido no se repite
    avisoActivo = true;
    avisoPendiente = true;               // NUEVO: avisar también a Railway
    Serial.println(">>> AVISO recibido");
  }
  server.send(200, "text/plain", "ok");
}

// ---------- Envío al servidor (función de la guía, sección 12.2) ----------
int enviarPost(const char* ruta, JsonDocument& doc) {
  WiFiClientSecure cliente;
  cliente.setInsecure();  // acepta el certificado sin verificarlo (ver guía)

  HTTPClient http;
  http.setTimeout(8000);
  http.begin(cliente, String(SERVER_URL) + ruta);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", API_KEY);

  String cuerpo;
  serializeJson(doc, cuerpo);
  int codigo = http.POST(cuerpo);
  Serial.printf("POST %s %s -> %d\n", ruta, cuerpo.c_str(), codigo);
  if (codigo > 0) {
    Serial.println(http.getString());
  } else {
    Serial.printf("Error: %s\n", http.errorToString(codigo).c_str());
  }
  http.end();
  return codigo;
}

void enviarLectura(int amplitud) {
  JsonDocument doc;
  doc["dispositivo"] = DISPOSITIVO;
  doc["sensor"] = "ruido";
  doc["valor"] = amplitud;
  doc["nivel"] = nivelDe(amplitud);
  enviarPost("/api/lecturas", doc);
}

bool enviarAviso() {
  JsonDocument doc;
  doc["dispositivo"] = DISPOSITIVO;
  int codigo = enviarPost("/api/asistencia", doc);
  return codigo == 200 || codigo == 201;
}

// Pregunta al servidor si el aviso sigue activo.
// Devuelve 1 = sigue activo, 0 = la profesora lo atendió, -1 = no se pudo saber.
int consultarAviso() {
  WiFiClientSecure cliente;
  cliente.setInsecure();
  HTTPClient http;
  http.setTimeout(8000);
  http.begin(cliente, String(SERVER_URL) + "/api/estado");
  int codigo = http.GET();
  int resultado = -1;
  if (codigo == 200) {
    JsonDocument doc;
    if (!deserializeJson(doc, http.getString())) {
      resultado = doc["asistencia"]["activa"].as<bool>() ? 1 : 0;
    }
  } else {
    Serial.printf("GET /api/estado -> %d\n", codigo);
  }
  http.end();
  return resultado;
}

// Tarea aparte (núcleo 0): el envío a internet tarda 1-2 s y así no
// deja esperando al botón ni congela el semáforo.
void tareaNube(void* parametro) {
  unsigned long ultimaConsulta = 0;
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      if (avisoPendiente && enviarAviso()) {
        avisoPendiente = false;
        avisoEnServidor = true;
      }
      // Mientras el aviso está activo, revisar si la profesora ya lo atendió
      if (avisoEnServidor && millis() - ultimaConsulta >= CONSULTA_MS) {
        ultimaConsulta = millis();
        if (consultarAviso() == 0) {
          avisoEnServidor = false;
          avisoActivo = false;
          Serial.println(">>> La profesora atendió el aviso: LED apagado");
        }
      }
      // Mandar el promedio que calculó loop() (el mismo color que muestra el LED)
      bool enviar = false;
      int promedio = 0;
      portENTER_CRITICAL(&candado);
      if (hayPromedioNuevo) {
        promedio = promedioParaEnviar;
        hayPromedioNuevo = false;
        enviar = true;
      }
      portEXIT_CRITICAL(&candado);
      if (enviar) enviarLectura(promedio);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RGB1_R, OUTPUT);
  pinMode(RGB1_G, OUTPUT);
  pinMode(RGB1_B, OUTPUT);
  pinMode(LED_AVISO, OUTPUT);
  mostrarColor(0);              // parte en verde hasta tener el primer promedio
  inicioPromedio = millis();

  // CAMBIO: red propia para el botón + internet al mismo tiempo
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("AulaRuido", "aula12345");
  Serial.print("Red creada. IP: ");
  Serial.println(WiFi.softAPIP());
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  server.on("/aviso", atenderAviso);
  server.begin();

  xTaskCreatePinnedToCore(tareaNube, "nube", 12288, NULL, 1, NULL, 0);
}

void loop() {
  server.handleClient();

  int maximo = 0, minimo = 4095;
  unsigned long inicio = millis();
  while (millis() - inicio < VENTANA_MS) {
    int lectura = analogRead(PIN_SENSOR);
    if (lectura > maximo) maximo = lectura;
    if (lectura < minimo) minimo = lectura;
    server.handleClient();
  }
  int amplitud = maximo - minimo;

  // Sumar la medición al promedio de 10 s
  sumaAmplitud += amplitud;
  cuentaAmplitud++;

  // Cada 10 s: calcular el promedio, cambiar el LED y dejarlo listo para la nube.
  // Entre medio el LED no cambia, aunque haya un ruido fuerte de un instante.
  if (millis() - inicioPromedio >= INTERVALO_MS) {
    int promedio = sumaAmplitud / cuentaAmplitud;
    sumaAmplitud = 0;
    cuentaAmplitud = 0;
    inicioPromedio = millis();

    mostrarColor(promedio);

    portENTER_CRITICAL(&candado);
    promedioParaEnviar = promedio;
    hayPromedioNuevo = true;
    portEXIT_CRITICAL(&candado);

    Serial.printf("Promedio 10 s: %d -> %s", promedio, nivelDe(promedio));
    Serial.println(WiFi.status() == WL_CONNECTED ? "  | Internet: si" : "  | Internet: no");
  }

  // El LED de aviso queda encendido hasta que la profesora lo atiende
  digitalWrite(LED_AVISO, avisoActivo ? HIGH : LOW);

  delay(50);
}
