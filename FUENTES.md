# Fuentes del proyecto

Este archivo reúne el origen de todo lo que no escribió el equipo: plantillas, código base, librerías, servicios, documentación técnica y uso de inteligencia artificial.

## Plantillas y código base

| Elemento | Origen | Uso en el proyecto |
|---|---|---|
| Estructura de carpetas y README de cada carpeta | Plantilla [sebasdv/proyecto_final_TEI201](https://github.com/sebasdv/proyecto_final_TEI201), licencia MIT | Organización del repositorio |
| Servidor Express, conexión a PostgreSQL y función de envío del ESP32 | Guía del curso *"ESP32 conectado a una base de datos en la nube"* (TEI201) y su repositorio de ejemplo [ferjomendez/esp32-railway-demo](https://github.com/ferjomendez/esp32-railway-demo) | Base de `software/web/server.js` y de la función `enviarPost()` de `software/src/central_nube/central_nube.ino`. El equipo agregó el semáforo, los pedidos de ayuda, el acceso de la profesora, los motivos y el registro diario |
| Ordenamiento del repositorio final | Repositorio de ejemplo del curso [ferjomendez/Repo-Ejemplo-TEI201](https://github.com/ferjomendez/Repo-Ejemplo-TEI201) | Formato del README principal y de la documentación |

## Librerías del ESP32

Todas se usan desde Arduino IDE 2.x.

| Librería | Versión | Dónde se usa | Licencia | Fuente |
|---|---|---|---|---|
| WiFi.h | Incluida en el paquete ESP32 | Central y botón | LGPL-2.1 | Espressif, [arduino-esp32](https://github.com/espressif/arduino-esp32) |
| WebServer.h | Incluida en el paquete ESP32 | Central (recibe el botón y, en la v2, sirve la página local) | LGPL-2.1 | Espressif, arduino-esp32 |
| HTTPClient.h | Incluida en el paquete ESP32 | Botón y central con nube | LGPL-2.1 | Espressif, arduino-esp32 |
| WiFiClientSecure.h | Incluida en el paquete ESP32 | Central con nube (conexión HTTPS a Railway) | LGPL-2.1 | Espressif, arduino-esp32 |
| ArduinoJson | 7.x | Central con nube (arma y lee los mensajes JSON) | MIT | Benoît Blanchon, [arduinojson.org](https://arduinojson.org) |

## Librerías del servidor y de la página web

Las versiones exactas están en `software/web/package-lock.json`.

| Librería | Versión | Dónde se usa | Licencia | Sitio |
|---|---|---|---|---|
| Express | 4.22.3 | Servidor web y API | MIT | https://expressjs.com |
| pg (node-postgres) | 8.23.1 | Conexión a PostgreSQL | MIT | https://node-postgres.com |
| dotenv | 16.6.1 | Lee las claves del archivo `.env` en el computador | BSD-2-Clause | https://github.com/motdotla/dotenv |
| Chart.js | 4.4.1 (desde jsDelivr) | Gráficos de la página y del registro diario | MIT | https://www.chartjs.org |
| Fuentes Baloo 2 y Nunito | Google Fonts | Tipografía de la página | SIL OFL 1.1 | https://fonts.google.com |

## Herramientas de diseño de la caja

| Herramienta | Uso | Fuente |
|---|---|---|
| [boxes.py](https://github.com/florianfesti/boxes) (generador `ElectronicsBox`), licencia GPL-3.0 | Base de la caja de MDF con uniones dentadas, triángulos para atornillar la tapa y fondo con orejas para colgar. Se le agregaron las ventanas, el hoyo del micrófono, la salida del cable y los soportes de los LED | Florian Festi, https://boxes.hackerspace-bamberg.de |
| Onshape | Primer modelo 3D del semáforo y de la carcasa del botón (Avance 2) | https://www.onshape.com |
| Reglas de corte láser del DEC | Formato de los archivos: DXF en mm, rojo = corte, azul = grabado, sin líneas auxiliares | https://dec.uai.cl/impresion-3d-corte-laser/ |

## Servicios en la nube

| Servicio | Plan | Uso |
|---|---|---|
| [Railway](https://railway.com) | Prueba de 30 días | Aloja el servidor y la página, y la publica sola en cada `git push` |
| [Aiven for PostgreSQL](https://aiven.io/postgresql) | Free | Base de datos con las mediciones y los pedidos de ayuda |
| [Wokwi](https://wokwi.com/projects/476425261066381313) | Gratuito | Simulación del circuito |
| Laboratorio DEC UAI | Cortadora láser | Corte de las piezas de MDF y acrílico de la caja del semáforo |

## Documentación técnica consultada

- Hoja de datos del ESP32 (Espressif), para los pines ADC1 que siguen funcionando con WiFi encendido. https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
- Hoja de datos del ESP32-S3 (Espressif), para el botón del estudiante. https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- Ficha del módulo de sonido KY-038, para la salida analógica (AO) y su alimentación.
- Documentación de arduino-esp32 sobre el modo `WIFI_AP_STA`, que permite crear la red del botón y conectarse a internet al mismo tiempo.

## Referencias del problema

- Prevalencia del autismo: 1 de cada 31 niños de 8 años (CDC, informe ADDM 2025). https://www.cdc.gov/autism/data-research/
- Ley TEA N° 21.545 (Chile, 2023), que obliga a asegurar una educación inclusiva para estudiantes con TEA. https://www.bcn.cl/leychile/navegar?i=1190123
- Hipersensibilidad auditiva reportada por el 80,5 % de los padres de niños y adolescentes con TEA (estudio de la UFRGS, Brasil). https://lume.ufrgs.br/bitstream/handle/10183/265963/001177744.pdf
- Nivel de ruido recomendado en salas de clases: 35 dB de ruido de fondo (OMS, *Guidelines for Community Noise*, 1999). https://www.who.int/publications-detail-redirect/a68672
- El valor de **50 a 60 dB** como umbral de tolerancia en niños con TEA no se encontró en un estudio específico. Por eso el sistema **no usa decibeles**: los umbrales del semáforo se calibraron con pruebas en el lugar (ver `software/docs/captura_y_datos.md`).

## Uso de Inteligencia Artificial

| Herramienta | Para qué se usó | Qué se adaptó o cambió | Qué entiende el equipo |
|---|---|---|---|
| Claude (Anthropic), chat | Ayuda para escribir partes del código de los ESP32 (medición de amplitud, semáforo, botón con interrupción) | El equipo calibró los umbrales del micrófono con el Serial Monitor y el Serial Plotter, y ajustó los pines al cableado real | La amplitud es el máximo menos el mínimo que lee el micrófono en 50 ms; la interrupción evita perder presiones del botón |
| Claude (Anthropic), chat | Datos y fuentes para las diapositivas del problema (ODS 4 y 10) | El equipo eligió qué datos usar y redactó las diapositivas | — |
| Claude (Anthropic), chat | Diseño de la caja de MDF con boxes.py, archivos DXF según las reglas del DEC, soportes de los LED, guía de armado y BOM con precios de referencia | El equipo definió las medidas (10 × 17 × 8 cm), la ubicación de las luces y del micrófono, y cómo sujetar los LED; armó y probó la caja | Las uniones dentadas encajan sin pegamento y la tapa va atornillada para poder abrirla |
| Claude Code (Anthropic) | Seguir la guía de Railway y Aiven, escribir el servidor y la página web, adaptar el código del ESP32 para mandar datos a la nube, ordenar la documentación del repositorio | Se definió qué debía mostrar la página (semáforo visual, luz azul de solicitud de ayuda, acceso de la profesora, registro de los motivos por los que se solicitó y registro diario) | El ESP32 manda un POST con JSON cada 10 s; el servidor lo guarda en PostgreSQL y la página lo lee cada 3 s |

Todo lo que entregó la IA fue revisado y probado por el equipo con el prototipo real.
