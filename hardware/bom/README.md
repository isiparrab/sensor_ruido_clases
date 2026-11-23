# Lista de materiales (BOM) y conexiones

La lista completa con costos está en [BOM.csv](BOM.csv).

## Costo de referencia

Precios de tiendas chilenas consultados el 22-11-2026 (IVA incluido, sin despacho). Son **precios de referencia**, no lo que pagó el equipo: pueden cambiar y algunos productos estaban agotados.

| Componente | Cant. | Costo CLP | Referencia |
|---|---|---|---|
| ESP32 DevKit | 1 | 8.990 | [MCI Electronics](https://mcielectronics.cl/shop/product/76195/) |
| Sensor de sonido KY-038 | 1 | 2.990 | [Ukrabot](https://www.ukrabot.cl/producto/sensor-de-sonido-ky-038) |
| LED RGB de cátodo común | 1 | 100 | [Afel](https://afel.cl/products/diodo-led-rgb-5mm) |
| LED azul (pack de 10) | 1 | 506 | [Altronics](https://altronics.cl/led-5mm-azul) |
| ESP32-S3 SuperMini | 1 | 9.990 | [Afel](https://afel.cl/products/placa-esp32-s3-supermini-wifi-y-bluetooth) |
| Batería LiPo 3,7 V 500 mAh | 1 | 6.290 | [Mercado Libre](https://listado.mercadolibre.cl/bateria-3.7v-500mah) |
| Cables dupont (pack de 40) | 1 | 2.490 | [Maxelectronica](https://maxelectronica.cl/protoboard-cables/84-cable-conexion-dupont-40-unidades-largo-20cm-hembra-hembra.html) |
| Cargador USB de 5 V con cable | 1 | 6.990 | [Paris](https://www.paris.cl/cargador-usb-12w-con-cable-micro-usb-certificado-ps-MKX30I4R7Q.html) |
| MDF 3 mm 40 × 40 cm + acrílico 3 mm 30 × 40 cm | 1 | 5.350 | [Sodimac](https://www.sodimac.cl/sodimac-cl/b/mdf-3mm-dimensionado) ($1.390) + [DesignTec](https://www.designtec.cl/tienda/corte-y-grabado-laser/) ($3.960) |
| Tornillos M3 con tuerca | 4 | 260 | [Mechatronicstore](https://www.mechatronicstore.cl/20-piezas-tornillo-m3-con-tuerca/) (pack de 20 a $1.290) |
| **Total** | | **43.956** | |

No incluye el pulsador ni la carcasa impresa en 3D, porque no se encontró su precio. Tampoco incluye el servicio de corte láser, solo el material. Si el equipo compró algo a otro precio, conviene reemplazarlo por lo que pagó.

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
| Carcasa del botón | 1 | Impresión 3D |
| Tornillos de la tapa | 4 | — |

## Versión anterior (v2)

En la v2, el LED del semáforo usaba R 25, G 26 y B 27, y la luz de solicitud era un segundo LED RGB en blanco (GPIO 12, 13 y 14). El botón era una ESP32-C3 SuperMini con el pulsador en el GPIO 4 y su LED en el GPIO 8. Ver [software/src/README.md](../../software/src/README.md#diferencias-entre-versiones).
