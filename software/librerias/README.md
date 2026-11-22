# Librerías

Ninguna librería viene incluida en el repositorio. Esta lista indica qué instalar y con qué versión se desarrolló el proyecto. Las fuentes y licencias están en [FUENTES.md](../../FUENTES.md).

## ESP32 (Arduino IDE 2.x)

Primero se instala el paquete de placas **esp32 de Espressif Systems** desde *Herramientas > Placa > Gestor de placas*. Trae incluidas casi todas las librerías.

| Librería | Versión | Cómo se instala | Usada en | Obligatoria |
|---|---|---|---|---|
| WiFi | Incluida en el paquete ESP32 | Con el paquete de placas | Central, central con nube y botón | ✅ |
| WebServer | Incluida en el paquete ESP32 | Con el paquete de placas | Central y central con nube | ✅ |
| HTTPClient | Incluida en el paquete ESP32 | Con el paquete de placas | Botón y central con nube | ✅ |
| WiFiClientSecure | Incluida en el paquete ESP32 | Con el paquete de placas | Central con nube | ✅ |
| ArduinoJson | 7.x | *Herramientas > Gestionar bibliotecas*, buscar "ArduinoJson" de Benoît Blanchon | Central con nube | ✅ |

## Servidor (Node.js 18 o superior)

Se instalan con `npm install` dentro de `software/web`. Las versiones exactas quedan fijadas en `package-lock.json`.

| Librería | Versión | Para qué |
|---|---|---|
| express | 4.22.3 | Servidor web y API |
| pg | 8.23.1 | Conexión a PostgreSQL |
| dotenv | 16.6.1 | Leer las claves del archivo `.env` en el computador |

## Página web

No se instala nada: se carga desde internet al abrir la página.

| Librería | Versión | Desde |
|---|---|---|
| Chart.js | 4.4.1 | cdn.jsdelivr.net |
| Fuentes Baloo 2 y Nunito | — | Google Fonts |
