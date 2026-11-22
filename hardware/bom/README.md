# Lista de materiales (BOM) y conexiones

La lista completa con columnas de costo está en [BOM.csv](BOM.csv). **Faltan los precios**: el equipo debe completarlos con lo que pagó o con el precio de una tienda, y calcular el total.

## Módulo central (pizarra)

| Componente | Cant. | Conexión |
|---|---|---|
| ESP32 DevKit | 1 | — |
| Sensor de sonido KY-038 | 1 | AO → GPIO 34 |
| LED RGB del semáforo (cátodo común) | 1 | Ver tabla de pines por versión, "-" → GND |
| LED de aviso azul (luz de ayuda) | 1 | Ver tabla de pines por versión, "-" → GND |

## Módulo del estudiante (botón)

| Componente | Cant. | Conexión |
|---|---|---|
| ESP32-C3 SuperMini | 1 | — |
| Pulsador | 1 | GPIO 4 y GND (pull-up interno) |
| LED de confirmación | — | LED integrado de la placa (GPIO 8, activo en LOW) |

## Encapsulado

| Pieza | Cant. | Material |
|---|---|---|
| Carcasa del semáforo | 1 | Impresión 3D (PLA) |
| Carcasa del botón | 1 | Impresión 3D (PLA) |

## Pines por versión del código

El equipo debe confirmar cuál es la versión final. Ver [software/src/README.md](../../software/src/README.md#diferencias-entre-versiones).

| Señal | `central.ino` (v2) | `central_nube.ino` (v3) |
|---|---|---|
| Semáforo, rojo | GPIO 25 | GPIO 27 |
| Semáforo, verde | GPIO 26 | GPIO 26 |
| Semáforo, azul (no se usa) | GPIO 27 | GPIO 25 |
| Luz de ayuda | LED RGB #2 en blanco: GPIO 12, 13 y 14 | LED azul: GPIO 13 |
