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
| ESP32-C3 SuperMini (botón) | C++ con Arduino IDE 2.x | WiFi, HTTPClient |
| Servidor en Railway | JavaScript con Node.js 18 o superior | Express 4, pg 8, dotenv 16 |
| Base de datos | SQL | PostgreSQL en Aiven (plan Free) |
| Página web | HTML, CSS y JavaScript | Chart.js 4.4.1, sin otras dependencias |

## Cómo se conectan las piezas

```
[Botón ESP32-C3] ──WiFi local──► [ESP32 central] ──HTTPS──► [Servidor Express] ──SSL──► [PostgreSQL]
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
2. Placa **ESP32C3 Dev Module** (o *Nologo ESP32C3 Super Mini*), con *USB CDC On Boot* activado para ver el Monitor Serie.
3. Subir. El LED integrado parpadea mientras busca la red y queda fijo cuando la solicitud está activa.

### 4. Servidor y página web

Ya están publicados en https://semaforoenlinea.up.railway.app. Para correrlos en el computador o desplegarlos de nuevo, seguir [web/README.md](web/README.md).

## Calidad del código

- Cada archivo empieza con un comentario que explica qué hace, sus librerías y su historial.
- Los datos que cambian según el lugar (red WiFi, claves, umbrales y pines) están juntos al inicio de cada archivo como constantes.
- El ESP32 central separa el trabajo en dos núcleos: el núcleo 1 mide el ruido y atiende el botón; el núcleo 0 hace los envíos HTTPS, que tardan 1 a 2 s, para que el semáforo no se congele.
- Si falla el envío de un aviso, se reintenta cada 5 s sin bloquear al resto.
- El servidor valida todo lo que recibe (números, colores y motivos permitidos) y usa consultas SQL con parámetros, nunca texto pegado.
- Las claves viven en variables de entorno (`.env` en el computador y *Variables* en Railway). El `.env` está en `.gitignore`.
- La página escapa todo el texto antes de mostrarlo, para que una nota no pueda inyectar código.

Nombres: variables y funciones en `camelCase` y en español, constantes en `MAYUSCULAS_CON_GUION`.
