# Software

Esta carpeta reúne todo el código del sistema y su documentación. Está dividido según dónde corre cada pieza.

## Carpetas

| Carpeta | Contenido |
|---|---|
| [src/](src/README.md) | Código Arduino de los ESP32: central de la pizarra, botón del estudiante, versión con nube y pruebas |
| [web/](web/README.md) | Servidor Node.js y página web, desplegados en Railway |
| [docs/](docs/README.md) | Captura y datos, API, manual de operación y capturas de la página |
| [librerias/](librerias/README.md) | Dependencias con sus versiones |

## Stack tecnológico

| Equipo | Lenguaje | Piezas principales |
|---|---|---|
| ESP32 DevKit (central, pizarra) | C++ con Arduino IDE 2.x | WiFi, WebServer, HTTPClient, WiFiClientSecure, ArduinoJson 7 |
| ESP32-S3 SuperMini (botón) | C++ con Arduino IDE 2.x | WiFi, HTTPClient |
| Servidor en Railway | JavaScript con Node.js 18 o superior | Express 4, pg 8, dotenv 16 |
| Base de datos | SQL | PostgreSQL en Aiven (plan Free) |
| Página web | HTML, CSS y JavaScript | Chart.js 4.4.1, sin otras dependencias |

## Cómo se conectan las piezas

```
[Botón ESP32-S3] ──WiFi local──► [ESP32 central] ──HTTPS──► [Servidor Express] ──SSL──► [PostgreSQL]
                                     │                              ▲
                                     └─► LED RGB + luz de ayuda      └── Página web (navegador)
```

1. El ESP32 central mide el ruido cada ~100 ms y enciende el LED RGB en verde, amarillo o rojo.
2. Cada 10 s manda al servidor el promedio del ruido y su color (`POST /api/lecturas`).
3. Cuando el botón del estudiante avisa, enciende la luz de ayuda y avisa al servidor (`POST /api/asistencia`).
4. Mientras la ayuda está activa, pregunta cada 3 s si la profesora ya la atendió (`GET /api/estado`). Cuando la profesora marca "Ya atendí" en la página, el ESP32 apaga la luz.
5. La página consulta el estado cada 3 s y muestra el semáforo, la ayuda, el resumen del día y el registro diario.

El ESP32 nunca se conecta directo a la base de datos: siempre pasa por el servidor, que revisa la clave (`x-api-key`).

## Instalación

### 1. Arduino IDE y placas

1. Instalar [Arduino IDE 2.x](https://www.arduino.cc/en/software).
2. En *Archivo > Preferencias > URLs adicionales de gestor de placas*, agregar `https://espressif.github.io/arduino-esp32/package_esp32_index.json`.
3. En *Herramientas > Placa > Gestor de placas*, instalar **esp32 de Espressif Systems**.
4. En *Herramientas > Gestionar bibliotecas*, instalar **ArduinoJson** de Benoît Blanchon, versión 7.

### 2. Cargar el ESP32 central

1. Abrir `src/central_nube/central_nube.ino`.
2. Completar los datos marcados con `CAMBIAR ESTOS DATOS`:
   - `WIFI_SSID` y `WIFI_PASS`: red con internet de 2.4 GHz, por ejemplo el hotspot de un celular.
   - `API_KEY`: la misma clave configurada en Railway.
3. Placa **ESP32 Dev Module**, elegir el puerto y subir.
4. Abrir el Monitor Serie a 115200 baudios. Cada 10 s debe aparecer `POST /api/lecturas ... -> 201`.

> **Nunca subir a GitHub el `.ino` con la clave del WiFi ni la `API_KEY` reales.**

### 3. Cargar el botón del estudiante

1. Abrir `src/boton_estudiante/boton_estudiante.ino`.
2. Placa **ESP32S3 Dev Module**, con *USB CDC On Boot* activado para ver el Monitor Serie.
3. Subir. El LED de la placa parpadea en azul mientras busca la red `AulaRuido`. Al apretar el botón se pone verde 3 s si el aviso llegó, o rojo si no llegó.
4. Si `rgbLedWrite` no compila, cambiarlo por `neopixelWrite` (depende de la versión del paquete ESP32).

### 4. Servidor y página web

Ya están publicados en https://semaforoenlinea.up.railway.app. Para correrlos en el computador o desplegarlos de nuevo, seguir [web/README.md](web/README.md).

## Calidad del código

- **Cada archivo explica qué hace.** Al inicio de cada archivo hay un comentario que cuenta para qué sirve, qué librerías usa y cómo fue cambiando.
- **Los datos que se ajustan están juntos al principio.** La red WiFi, las claves, los umbrales del ruido y los pines están arriba del código, así se cambian fácil sin buscar por todo el archivo.
- **El semáforo no se congela al mandar datos.** El ESP32 tiene dos "cerebros" (núcleos). Uno mide el ruido y atiende el botón; el otro manda los datos a internet, que demora 1 a 2 segundos. Así el semáforo sigue funcionando mientras se envía.
- **Si un aviso no llega, se vuelve a intentar.** Si falla el envío de un pedido de ayuda, el ESP32 lo reintenta cada 5 segundos hasta que llega.
- **El servidor solo guarda datos que tienen sentido.** Revisa que el ruido sea un número y que el color sea verde, amarillo o rojo. Si llega algo raro, no lo guarda.
- **Las contraseñas no están en el código.** Se guardan en un archivo aparte (`.env`) que nunca se sube a GitHub, y en la configuración de Railway. Por eso el repositorio puede ser público sin mostrar las claves.
- **Nadie puede romper la página escribiendo cosas raras.** Si alguien escribe código en una nota, la página lo muestra como texto normal y no lo ejecuta.

Los nombres en el código siguen siempre la misma regla, para que sea fácil de leer: las variables y funciones juntan palabras con mayúscula al medio (`enviarLectura`, `avisoActivo`) y las constantes van todas en mayúscula (`UMBRAL_ROJO`).
