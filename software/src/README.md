# Código fuente de los ESP32

| Carpeta | Placa | Versión | Qué hace |
|---|---|---|---|
| [central/](central/central.ino) | ESP32 DevKit | v2 (Avance 2) | Semáforo en el LED RGB #1, crea la red WiFi `SemaforoAula`, recibe el botón y enciende la luz de solicitud. Muestra una página local en `http://192.168.4.1` |
| [central_nube/](central_nube/central_nube.ino) | ESP32 DevKit | v3 | Lo mismo que la central, pero además se conecta a internet y manda las mediciones y los pedidos de ayuda a la página en Railway. La luz de ayuda se apaga cuando la profesora la marca como atendida en la página |
| [boton_estudiante/](boton_estudiante/boton_estudiante.ino) | ESP32-C3 SuperMini | v2 | Botón del escritorio. Envía la solicitud por WiFi a la central y confirma con su LED |
| [pruebas/prueba_sensor_un_led/](pruebas/prueba_sensor_un_led/prueba_sensor_un_led.ino) | ESP32 DevKit | Prueba previa a la v2 | Prueba del micrófono con un solo LED RGB, usada para calibrar los umbrales (de 15 / 1000 se pasó a 17 / 42) |

## Cómo mide el ruido

El KY-038 entrega una señal analógica (pin AO) que el ESP32 lee en el GPIO 34, un pin ADC1 que sigue funcionando con el WiFi encendido. Durante una ventana de 50 ms se guarda el valor máximo y el mínimo, y la **amplitud** es la diferencia entre ambos. Una sala más ruidosa da una amplitud mayor.

```
amplitud <= UMBRAL_AMARILLO          → verde
UMBRAL_AMARILLO < amplitud <= UMBRAL_ROJO → amarillo (rojo + verde)
amplitud > UMBRAL_ROJO               → rojo
```

Los umbrales se calibraron mirando la amplitud en el Monitor Serie. Son unidades del conversor del ESP32 (0 a 4095), **no decibeles**: el KY-038 no está calibrado para medir dB.

## Diferencias entre versiones

Estas diferencias existen hoy en el código. **El equipo debe confirmar cuál es la versión final** antes de la entrega; este documento no cambia ningún valor.

| Dato | `central/central.ino` | `central_nube/central_nube.ino` |
|---|---|---|
| Umbrales amarillo / rojo | 17 / 42 | 25 / 50 |
| Pines del LED RGB del semáforo | R 25, G 26, B 27 | R 27, G 26, B 25 (corregido porque los colores salían cambiados en el prototipo) |
| Luz de solicitud | LED RGB #2 en **blanco** (pines 12, 13 y 14) | Un LED en el pin 13 (luz **azul**) |
| Red WiFi que crea para el botón | `SemaforoAula` | `AulaRuido` |
| Ruta que recibe el botón | `/boton` | `/aviso` |
| Cómo se apaga la luz de solicitud | Desde la página local o con el mismo botón | Solo la profesora, desde la página en Railway con su clave |

Además, el botón (`boton_estudiante.ino`) está configurado para la red `SemaforoAula` y la ruta `/boton` de la v2. Para usarlo con `central_nube`, ambas deben coincidir.

## Monitor Serie

Todos los programas usan 115200 baudios. La central con nube imprime la amplitud de cada medición, si hay internet, y el resultado de cada envío (`POST /api/lecturas ... -> 201`).
