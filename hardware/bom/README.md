# Lista de materiales (BOM) y conexiones

La lista completa con columnas de costo está en [BOM.csv](BOM.csv). **Faltan los precios**: el equipo debe completarlos con lo que pagó o con el precio de una tienda, y calcular el total.

## Módulo central (pizarra)

| Componente | Cant. | Conexión |
|---|---|---|
| ESP32 DevKit | 1 | Alimentación por USB |
| Sensor de sonido KY-038 | 1 | AO → GPIO 34 |
| LED RGB del semáforo (cátodo común) | 1 | R → GPIO 27, G → GPIO 26, B → GPIO 25 (no se usa), "-" → GND |
| LED de aviso azul (luz de ayuda) | 1 | GPIO 13, "-" → GND |

## Módulo del estudiante (botón)

| Componente | Cant. | Conexión |
|---|---|---|
| ESP32-S3 SuperMini | 1 | — |
| Pulsador | 1 | GPIO 5 y GND (pull-up interno) |
| LED de confirmación | — | LED RGB integrado de la placa (GPIO 48) |
| Batería LiPo de 3,7 V | 1 | Pines B+ y B- de la placa |

## Encapsulado

| Pieza | Cant. | Material |
|---|---|---|
| Caja del semáforo, 10 × 17 × 8 cm | 1 | MDF y acrílico cortados con láser, tapa atornillada |
| Carcasa del botón | 1 | Por confirmar |
| Tornillos de la tapa | Por confirmar | — |

## Versión anterior (v2)

En la v2, el LED del semáforo usaba R 25, G 26 y B 27, y la luz de solicitud era un segundo LED RGB en blanco (GPIO 12, 13 y 14). El botón era una ESP32-C3 SuperMini con el pulsador en el GPIO 4 y su LED en el GPIO 8. Ver [software/src/README.md](../../software/src/README.md#diferencias-entre-versiones).
