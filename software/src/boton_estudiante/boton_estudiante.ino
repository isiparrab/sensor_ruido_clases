
/*
  =====================================================================
  SEMÁFORO DE RUIDO — BOTÓN DEL ESTUDIANTE  (v3)
  TEI201 Taller de Diseño en Ingeniería — Avance #3
  Placa: ESP32-S3 SuperMini (alimentada con batería LiPo 3,7 V en B+/B-)
  =====================================================================
  QUÉ HACE:
   - Se conecta a la red "AulaRuido" que crea el ESP32 central (pizarra).
   - Cuando el estudiante aprieta el botón, manda un aviso a
     http://192.168.4.1/aviso. El central enciende la luz de ayuda y
     lo registra en la nube; la profesora la apaga desde la página web.
   - El LED RGB de la placa (GPIO 48) le confirma al estudiante:
       azul parpadeando = buscando la red
       verde 3 s        = el aviso llegó
       rojo 3 s         = el aviso no llegó (volver a apretar)

  CAMBIOS v2 -> v3:
   - Placa ESP32-C3 -> ESP32-S3 SuperMini: el LED ahora es RGB en el pin 48.
   - Red y ruta iguales a central_nube.ino (AulaRuido / aula12345 / /aviso).
   - Se quitó la consulta /solicitud: la luz ahora la apaga solo la profesora.

  LIBRERÍAS: WiFi.h y HTTPClient.h (incluidas en el paquete ESP32).
  En Arduino IDE: Board = "ESP32S3 Dev Module", USB CDC On Boot = Enabled.
  =====================================================================
*/

#include <WiFi.h>
#include <HTTPClient.h>

// ---------------------- RED DEL ESP32 CENTRAL ------------------------
const char* WIFI_NOMBRE = "AulaRuido";      // igual que en central_nube.ino
const char* WIFI_CLAVE  = "aula12345";
const char* URL_AVISO   = "http://192.168.4.1/aviso";

// ---------------------- PINES ----------------------------------------
const int PIN_BOTON = 5;     // botón entre este pin y GND (pull-up interno)
const int PIN_LED   = 48;    // LED RGB integrado de la ESP32-S3 SuperMini

// ---------------------- VARIABLES ------------------------------------
volatile bool botonPresionado = false;   // lo marca la interrupción
volatile unsigned long ultimaPresion = 0;
const unsigned long ANTIREBOTE_MS = 300; // evita lecturas dobles

// =====================================================================

// Interrupción: se ejecuta apenas se aprieta el botón, aunque la placa
// esté ocupada, así ninguna presión se pierde.
void IRAM_ATTR alApretar() {
  unsigned long ahora = millis();
  if (ahora - ultimaPresion > ANTIREBOTE_MS) {
    ultimaPresion = ahora;
    botonPresionado = true;
  }
}

// Enciende el LED RGB de la placa con brillo bajo (0-255 por color).
void led(int r, int g, int b) {
  rgbLedWrite(PIN_LED, r, g, b);   // si no compila, usar neopixelWrite(...)
}

// Se conecta (o reconecta) a la red del central. Azul parpadeando = buscando.
void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("Conectando a ");
  Serial.println(WIFI_NOMBRE);
  WiFi.begin(WIFI_NOMBRE, WIFI_CLAVE);
  bool encendido = false;
  while (WiFi.status() != WL_CONNECTED) {
    encendido = !encendido;
    led(0, 0, encendido ? 30 : 0);
    delay(200);
  }
  led(0, 0, 0);
  Serial.print("Conectado. IP del botón: ");
  Serial.println(WiFi.localIP());
}

// Manda el aviso al central. Devuelve true si llegó (respuesta 200).
bool enviarAviso() {
  HTTPClient http;
  http.setTimeout(2000);
  http.begin(URL_AVISO);
  int codigo = http.GET();
  http.end();
  Serial.printf("GET /aviso -> %d\n", codigo);
  return codigo == 200;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  led(0, 0, 0);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);   // sin ahorro de energía = respuesta rápida
  conectarWiFi();

  attachInterrupt(digitalPinToInterrupt(PIN_BOTON), alApretar, FALLING);
}

void loop() {
  conectarWiFi();   // si se cae la red, se reconecta solo

  if (botonPresionado) {
    botonPresionado = false;
    Serial.println("Botón presionado -> enviando aviso...");
    if (enviarAviso()) {
      led(0, 40, 0);    // verde: el aviso llegó
    } else {
      led(40, 0, 0);    // rojo: no llegó, volver a apretar
    }
    delay(3000);
    led(0, 0, 0);
  }
  delay(5);
}
