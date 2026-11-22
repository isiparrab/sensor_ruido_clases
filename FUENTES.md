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

## Servicios en la nube

| Servicio | Plan | Uso |
|---|---|---|
| [Railway](https://railway.com) | Prueba de 30 días | Aloja el servidor y la página, y la publica sola en cada `git push` |
| [Aiven for PostgreSQL](https://aiven.io/postgresql) | Free | Base de datos con las mediciones y los pedidos de ayuda |
| [Wokwi](https://wokwi.com/projects/476425261066381313) | Gratuito | Simulación del circuito |

## Documentación técnica consultada

- Hoja de datos del ESP32 (Espressif), para los pines ADC1 que siguen funcionando con WiFi encendido. https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
- Hoja de datos del ESP32-C3 (Espressif), para el botón del estudiante. https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf
- Ficha del módulo de sonido KY-038, para la salida analógica (AO) y su alimentación.
- Documentación de arduino-esp32 sobre el modo `WIFI_AP_STA`, que permite crear la red del botón y conectarse a internet al mismo tiempo.

## Referencias del problema

- Umbral de tolerancia acústica de 50 a 60 dB en niños con TEA e hipersensibilidad auditiva, citado en la descripción del proyecto. **Falta agregar aquí la referencia bibliográfica.**

## Uso de Inteligencia Artificial

| Herramienta | Para qué se usó | Qué se adaptó o cambió |
|---|---|---|---|
| Claude (Anthropic) | Ayuda para escribir partes del código de los ESP32 | El equipo calibró los umbrales (17 y 42) con el Serial Monitor y ajustó los niveles del micrófono |
| Claude Code (Anthropic) | Seguir la guía de Railway y Aiven, escribir el servidor y la página web, adaptar el código del ESP32 para mandar datos a la nube, ordenar la documentación del repositorio | Se definió qué debía mostrar la página (semáforo visual, luz azul de solicitud de ayuda, acceso de la profesora, registro de los motivos por la que se solicitó y registro diario) |

Todo lo que entregó la IA fue revisado y probado por el equipo con el prototipo real.
