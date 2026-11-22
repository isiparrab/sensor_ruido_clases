# Página web del semáforo (Railway + Aiven)

Página pública: **https://semaforoenlinea.up.railway.app**

La página muestra en tiempo real el estado del semáforo de la sala y permite a la
profesora apagar la luz azul de ayuda desde su computador o celular.

## Arquitectura

```
Botón del estudiante ──WiFi "AulaRuido"──► ESP32 central (semáforo)
                                               │  HTTPS (JSON)
                                               ▼
                          Servidor Express en Railway  ──SQL con SSL──►  PostgreSQL en Aiven
                                               ▲
                                               │  GET /api/estado
                          Navegador (sala / profesora)
```

- **ESP32 central** (`software/src/central_nube/central_nube.ino`): mide el ruido con el
  KY-038, enciende el LED RGB (verde, amarillo o rojo) y cada 10 s manda el promedio al
  servidor. Cuando el botón del estudiante avisa, enciende el LED azul y avisa al servidor.
- **Servidor** (`server.js`): Node.js + Express. Guarda las mediciones y los pedidos de
  ayuda en PostgreSQL y entrega la página.
- **Página** (`public/index.html`): se actualiza sola cada 3 s.

El ESP32 nunca se conecta directo a la base de datos: siempre pasa por el servidor.

## Qué muestra la página

- Un semáforo grande con el color actual y un mensaje para los estudiantes.
- La tarjeta de **ayuda** (luz azul): se activa cuando un estudiante presiona el botón.
- **Acceso profesora**: con su clave, la profesora marca "Ya atendí" y el ESP32 apaga el LED azul.
- **Hoy en la sala**: porcentaje del día en cada color y cantidad de pedidos de ayuda.
- Gráfico del ruido y registro de mediciones.
- Aviso de **"Sin señal"** si el semáforo no manda datos por más de 60 s.

## API

| Ruta | Método | Quién la usa | Protección |
|------|--------|--------------|------------|
| `/api/salud` | GET | Prueba de conexión con la base | — |
| `/api/lecturas` | POST | ESP32: `{"dispositivo","sensor":"ruido","valor","nivel"}` | `x-api-key` |
| `/api/lecturas?limite=N` | GET | Página: últimas mediciones | — |
| `/api/estado` | GET | Página y ESP32: color actual, ayuda activa, resumen del día | — |
| `/api/asistencia` | POST | ESP32: un estudiante pidió ayuda | `x-api-key` |
| `/api/profe/entrar` | POST | Página: comprobar la clave de la profesora | `x-profe-clave` |
| `/api/asistencia/atender` | POST | Página: la profesora apaga la luz azul | `x-profe-clave` |

`nivel` puede ser `verde`, `amarillo` o `rojo`. Las tablas `lecturas` y `asistencias`
se crean solas al arrancar el servidor.

## Variables de entorno

Ver `.env.example`. **Nunca subir el archivo `.env` con las claves reales.**

| Variable | Qué es |
|----------|--------|
| `DATABASE_URL` | Service URI de PostgreSQL en Aiven |
| `DB_CA_CERT` | Certificado CA de Aiven |
| `API_KEY` | Clave compartida con el ESP32 (la misma va en el `.ino`) |
| `PROFE_CLAVE` | Clave de la profesora para apagar la luz azul |

## Probar en el computador

Requisitos: Node.js 18 o superior.

```bash
cd software/web
cp .env.example .env      # y completar con los datos reales
npm install
npm start                 # en PowerShell: npm.cmd start
```

Abrir http://localhost:3000 y http://localhost:3000/api/salud (debe responder
`{"ok":true,"db":"conectada"}`).

## Despliegue

El servidor está desplegado en **Railway** (plan de prueba de 30 días, desde el 20/11/2026)
y la base de datos en **Aiven PostgreSQL** (plan Free). Se siguió la guía
*"ESP32 conectado a una base de datos en la nube"* del curso TEI201.
