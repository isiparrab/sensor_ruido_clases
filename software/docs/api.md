# API del servidor

Servidor: `https://semaforoenlinea.up.railway.app` (código en [software/web/server.js](../web/server.js)). Todas las respuestas son JSON.

## Protección

| Encabezado | Quién lo usa | Para qué |
|---|---|---|
| `x-api-key` | ESP32 central | Escribir mediciones y pedidos de ayuda |
| `x-profe-clave` | Página, en modo profesora | Apagar la luz de ayuda, anotar motivos y ver motivos y notas |

Las claves se configuran en las variables de entorno `API_KEY` y `PROFE_CLAVE`. Si la clave no coincide, la respuesta es `401`.

## Rutas

| Ruta | Método | Quién la usa | Protección |
|---|---|---|---|
| `/api/salud` | GET | Prueba de conexión con la base | — |
| `/api/lecturas` | POST | ESP32: guarda una medición | `x-api-key` |
| `/api/lecturas?limite=N` | GET | Página: últimas N mediciones (máximo 500) | — |
| `/api/estado` | GET | Página y ESP32: color actual, ayuda activa y resumen del día | — |
| `/api/asistencia` | POST | ESP32: un estudiante pidió ayuda | `x-api-key` |
| `/api/profe/entrar` | POST | Página: comprobar la clave de la profesora | `x-profe-clave` |
| `/api/asistencia/atender` | POST | Página: apagar la luz de ayuda y guardar el motivo | `x-profe-clave` |
| `/api/asistencia/:id/motivo` | POST | Registro: anotar o cambiar el motivo de un pedido | `x-profe-clave` |
| `/api/calendario?mes=AAAA-MM` | GET | Registro: resumen de cada día del mes | — |
| `/api/reporte?fecha=AAAA-MM-DD` | GET | Registro: reporte de un día | — (motivos solo con `x-profe-clave`) |

## Ejemplos

**El ESP32 manda una medición**

```http
POST /api/lecturas
x-api-key: ••••
Content-Type: application/json

{"dispositivo": "semaforo-sala", "sensor": "ruido", "valor": 43, "nivel": "amarillo"}
```

Respuesta `201`: `{"ok": true, "id": 354}`. `nivel` debe ser `verde`, `amarillo` o `rojo`; si no, la respuesta es `400`.

**Estado actual** (`GET /api/estado`)

```json
{
  "nivel": "amarillo",
  "valor": 43,
  "actualizado_en": "2026-11-22T12:01:03.896Z",
  "conectado": false,
  "asistencia": { "activa": false },
  "hoy": { "verde": 2, "amarillo": 275, "rojo": 55, "ayudas": 2 },
  "ahora": "2026-11-22T21:22:40.137Z"
}
```

`conectado` es `false` si la última medición tiene más de 60 s. El ESP32 lee `asistencia.activa` para saber cuándo apagar la luz de ayuda.

**La profesora atiende un pedido**

```http
POST /api/asistencia/atender
x-profe-clave: ••••
Content-Type: application/json

{"motivo": "sensorial", "nota": "le molestaba el ruido del patio"}
```

`motivo` puede ser `actividad`, `sensorial`, `emocion`, `companero`, `personal` u `otro`, o ir vacío. La nota se corta en 300 caracteres.
