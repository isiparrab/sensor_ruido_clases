/*
  =====================================================================
  SEMÁFORO SOCIOEMOCIONAL DE AULA — BOTÓN DE ESCRITORIO  (v2)
  TEI201 Taller de Diseño en Ingeniería — Avance #2
  Placa: ESP32 mini (ej. ESP32-C3 SuperMini)
  =====================================================================
  QUÉ HACE:
   - Se conecta a la red WiFi "SemaforoAula" que crea el dispositivo central.
   - Cuando el estudiante aprieta el botón, envía un mensaje WiFi al central
     para encender (o cancelar) la luz blanca de la pizarra.
   - Su LED propio confirma al estudiante que el mensaje llegó.
     LED encendido fijo = solicitud activa
     LED parpadeando rápido = buscando la red WiFi

  ITERACIÓN v1 -> v2:
   Problema: a veces había que apretar varias veces para que se prendiera
   la luz blanca (respuesta lenta / presiones perdidas).
   Causas: ahorro de energía del WiFi activado, antena débil de la placa
   mini y presiones que ocurrían mientras la placa estaba ocupada.
   Solución: se desactiva el ahorro de energía del WiFi, se ajusta la
   potencia de transmisión y el botón se detecta con una interrupción,
   así ninguna presión se pierde.

  LIBRERÍAS: WiFi.h y HTTPClient.h (incluidas con el paquete ESP32).
  =====================================================================
*/

#include <WiFi.h>
#include <HTTPClient.h>

// ---------------------- RED DEL DISPOSITIVO CENTRAL ------------------
const char* WIFI_NOMBRE = "SemaforoAula";
const char* WIFI_CLAVE  = "aula2026";
const char* URL_BOTON     = "http://192.168.4.1/boton";      // alterna la solicitud
const char* URL_SOLICITUD = "http://192.168.4.1/solicitud";  // consulta el estado

// ---------------------- PINES ----------------------------------------
const int PIN_BOTON = 4;       // botón entre este pin y GND (sin resistencia)
const int PIN_LED   = 8;       // LED de confirmación (integrado en la C3 SuperMini)
const bool LED_ACTIVO_BAJO = true;  // la C3 SuperMini enciende su LED con LOW

// ---------------------- VARIABLES ------------------------------------
volatile bool botonPresionado = false;     // lo marca la interrupción
volatile unsigned long ultimaPresion = 0;
const unsigned long ANTIREBOTE_MS = 300;   // evita lecturas dobles del botón
unsigned long ultimaConsulta = 0;
bool solicitudActiva = false;

// =====================================================================

// Interrupción: se ejecuta al instante cuando se aprieta el botón,
// aunque la placa esté ocupada haciendo otra cosa.
void IRAM_ATTR alApretar() {
  unsigned long ahora = millis();
  if (ahora - ultimaPresion > ANTIREBOTE_MS) {
    ultimaPresion = ahora;
    botonPresionado = true;
  }
}

void ledConfirmacion(bool encendido) {
  digitalWrite(PIN_LED, LED_ACTIVO_BAJO ? !encendido : encendido);
}

// Se conecta (o reconecta) a la red del dispositivo central.
void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("Conectando a ");
  Serial.println(WIFI_NOMBRE);
  WiFi.begin(WIFI_NOMBRE, WIFI_CLAVE);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);   // mejora la conexión en placas mini
  bool led = false;
  while (WiFi.status() != WL_CONNECTED) {
    led = !led;
    ledConfirmacion(led);               // parpadeo = buscando red
    delay(150);
  }
  Serial.print("Conectado. IP de este botón: ");
  Serial.println(WiFi.localIP());
  ledConfirmacion(solicitudActiva);
}

// Envía una petición al central y devuelve su respuesta ("ON"/"OFF").
String pedir(const char* url) {
  HTTPClient http;
  http.setTimeout(1500);
  http.begin(url);
  int codigo = http.GET();
  String respuesta = "";
  if (codigo == 200) respuesta = http.getString();
  else {
    Serial.print("Error de comunicación: ");
    Serial.println(codigo);
  }
  http.end();
  return respuesta;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  ledConfirmacion(false);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);                 // sin ahorro de energía = respuesta rápida
  conectarWiFi();

  attachInterrupt(digitalPinToInterrupt(PIN_BOTON), alApretar, FALLING);
}

void loop() {
  conectarWiFi();   // si se cae la conexión, se reconecta solo

  // 1. Si se apretó el botón, avisar al central
  if (botonPresionado) {
    botonPresionado = false;
    Serial.println("Botón presionado -> enviando por WiFi...");
    String r = pedir(URL_BOTON);
    if (r == "ON" || r == "OFF") {
      solicitudActiva = (r == "ON");
      ledConfirmacion(solicitudActiva);
      Serial.println(solicitudActiva ? "Central confirmó: solicitud ACTIVA"
                                     : "Central confirmó: solicitud cancelada");
    }
  }

  // 2. Cada 3 s preguntar si la solicitud sigue activa
  //    (por si el profesor la marcó como atendida desde la página)
  if (!botonPresionado && millis() - ultimaConsulta > 3000) {
    ultimaConsulta = millis();
    String r = pedir(URL_SOLICITUD);
    if (r == "ON" || r == "OFF") {
      solicitudActiva = (r == "ON");
      ledConfirmacion(solicitudActiva);
    }
  }
  delay(5);
}
