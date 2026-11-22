/*
  =====================================================================
  PRUEBA DEL SENSOR DE RUIDO — UN SOLO LED RGB
  TEI201 Taller de Diseño en Ingeniería — Testing previo al Avance #2
  Placa: ESP32 DevKit conectado al computador (Serial Monitor)
  =====================================================================
  OBJETIVO: probar que el KY-038 mide el ruido y que el LED RGB cambia
  de color según la amplitud, antes de integrar WiFi y botón.

  RESULTADO: el Serial Monitor mostraba el color correcto, pero el
  amarillo no se veía en el LED: pasaba de verde a rojo muy rápido.
  Conclusión: había que ajustar los umbrales. Después de calibrar con
  el Serial Monitor quedaron en AMARILLO = 17 y ROJO = 42 (central.ino).
  =====================================================================
*/

const int PIN_SENSOR = 34;   // AO del KY-038 (pin ADC1)
const int VENTANA_MS = 50;   // duración de cada medición de amplitud

// Pines del LED RGB
const int RGB1_R = 25;
const int RGB1_G = 26;
const int RGB1_B = 27;

// Umbrales de prueba (sin calibrar)
const int UMBRAL_AMARILLO = 15;
const int UMBRAL_ROJO     = 1000;

void setup() {
  Serial.begin(115200);
  pinMode(RGB1_R, OUTPUT);
  pinMode(RGB1_G, OUTPUT);
  pinMode(RGB1_B, OUTPUT);
  Serial.println("CODIGO VERSION 2 INICIADO");
}

void loop() {
  // 1. Medir amplitud: diferencia entre el valor máximo y mínimo
  //    que lee el micrófono durante VENTANA_MS milisegundos
  int maximo = 0;
  int minimo = 4095;
  unsigned long inicio = millis();
  while (millis() - inicio < VENTANA_MS) {
    int lectura = analogRead(PIN_SENSOR);
    if (lectura > maximo) maximo = lectura;
    if (lectura < minimo) minimo = lectura;
  }
  int amplitud = maximo - minimo;

  // 2. Apagar el LED antes de elegir el nuevo color
  digitalWrite(RGB1_R, LOW);
  digitalWrite(RGB1_G, LOW);
  digitalWrite(RGB1_B, LOW);

  // 3. Elegir color según la amplitud
  String color;
  if (amplitud <= UMBRAL_AMARILLO) {
    digitalWrite(RGB1_G, HIGH);            // verde
    color = "VERDE";
  } else if (amplitud <= UMBRAL_ROJO) {
    digitalWrite(RGB1_R, HIGH);
    digitalWrite(RGB1_G, HIGH);            // rojo + verde = amarillo
    color = "AMARILLO";
  } else {
    digitalWrite(RGB1_R, HIGH);            // rojo
    color = "ROJO";
  }

  // 4. Mostrar resultado en el Serial Monitor
  Serial.print("Amplitud: ");
  Serial.print(amplitud);
  Serial.print(" -> Color: ");
  Serial.println(color);

  delay(200);   // una medición cada ~250 ms
}
