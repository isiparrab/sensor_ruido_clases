# Código fuente de los ESP32

## Versión final (v3)

| Carpeta | Placa | Qué hace |
|---|---|---|
| [central_nube/](central_nube/central_nube.ino) | ESP32 DevKit | Mide el ruido y lo muestra en el LED RGB del semáforo. Crea la red WiFi `AulaRuido` para el botón y, al mismo tiempo, se conecta a internet. Manda las mediciones y los pedidos de ayuda a la página en Railway. La luz de ayuda azul se apaga cuando la profesora la marca como atendida en la página |
| [boton_estudiante/](boton_estudiante/boton_estudiante.ino) | ESP32-S3 SuperMini con batería LiPo | Botón del escritorio. Se conecta a `AulaRuido`, manda el aviso a `http://192.168.4.1/aviso` y confirma con su LED RGB: azul parpadeando = buscando la red, verde = el aviso llegó, rojo = no llegó |

## Versiones anteriores (historial)

| Carpeta | Placa | Versión | Qué hacía |
|---|---|---|---|
| [central/](central/central.ino) | ESP32 DevKit | v2 (Avance 2) | Semáforo en el LED RGB #1, red WiFi propia `SemaforoAula` sin internet, luz de solicitud blanca (LED RGB #2) y página local en `http://192.168.4.1` |
| [pruebas/prueba_sensor_un_led/](pruebas/prueba_sensor_un_led/prueba_sensor_un_led.ino) | ESP32 DevKit | Prueba previa a la v2 | Prueba del micrófono con un solo LED RGB, usada para calibrar los umbrales (de 15 / 1000 se pasó a 17 / 42) |

## Cómo mide el ruido

El KY-038 entrega una señal analógica (pin AO) que el ESP32 lee en el GPIO 34, un pin ADC1 que sigue funcionando con el WiFi encendido. Durante una ventana de 50 ms se guarda el valor máximo y el mínimo, y la **amplitud** es la diferencia entre ambos. Una sala más ruidosa da una amplitud mayor.

```
amplitud <= 25          → verde
25 < amplitud <= 50     → amarillo (rojo + verde)
amplitud > 50           → rojo
```

Los umbrales se calibraron mirando la amplitud en el Monitor Serie. Son unidades del conversor del ESP32 (0 a 4095), **no decibeles**: el KY-038 no está calibrado para medir dB.

## Diferencias entre versiones

| Dato | v2 (`central.ino` + botón v2) | v3, final (`central_nube.ino` + botón v3) |
|---|---|---|
| Umbrales amarillo / rojo | 17 / 42 | 25 / 50 |
| Pines del LED RGB del semáforo | R 25, G 26, B 27 | R 26, G 27, B 25 (corregido porque los colores salían cambiados en el prototipo) |
| Luz de ayuda | LED RGB #2 en **blanco** (pines 12, 13 y 14) | LED **azul** en el pin 13 |
| Red WiFi para el botón | `SemaforoAula` | `AulaRuido` |
| Ruta que recibe el botón | `/boton` | `/aviso` |
| Placa del botón | ESP32-C3 SuperMini, botón en GPIO 4, LED en GPIO 8 | ESP32-S3 SuperMini con batería LiPo, botón en GPIO 5, LED RGB en GPIO 48 |
| Cómo se apaga la luz de ayuda | Desde la página local o con el mismo botón | Solo la profesora, desde la página en Railway con su clave |
| Datos | Se perdían al apagar | Se guardan en la nube |

## Monitor Serie

Todos los programas usan 115200 baudios. La central imprime cada 10 s el promedio del ruido y su color (`Promedio 10 s: 18 -> verde`), si hay internet y el resultado de cada envío (`POST /api/lecturas ... -> 201`). El botón imprime `GET /aviso -> 200` cuando el aviso llega.
