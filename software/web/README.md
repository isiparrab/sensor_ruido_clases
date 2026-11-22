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
- **Página del semáforo** (`public/index.html`): se actualiza sola cada 3 s.
- **Registro diario** (`public/registro.html`): calendario con el reporte de cada día.
- `public/comun.js` y `public/estilos.css`: código y estilos que usan las dos páginas.

El ESP32 nunca se conecta directo a la base de datos: siempre pasa por el servidor.

## Qué muestra la página

- Un semáforo grande con el color actual y un mensaje para los estudiantes.
- La tarjeta de **ayuda** (luz azul): se activa cuando un estudiante presiona el botón.
- **Acceso profesora**: con su clave, la profesora marca "Ya atendí", elige el **motivo** del
  pedido (duda con la actividad, mucho ruido o estímulos, emoción difícil, problema con un
  compañero, necesidad personal u otro) y puede escribir una nota. Luego el ESP32 apaga el LED azul.
- **Hoy en la sala**: porcentaje del día en cada color y cantidad de pedidos de ayuda.
- Gráfico del ruido y registro de mediciones.
- Aviso de **"Sin señal"** si el semáforo no manda datos por más de 60 s.

## Registro diario (`/registro.html`)

Todas las mediciones quedan guardadas en la base de datos (la página principal solo muestra
las 60 más recientes). El registro permite revisar cualquier día:

- **Calendario**: cada día muestra una barrita con el tiempo en verde/amarillo/rojo y la
  cantidad de pedidos de ayuda.
- **Reporte del día**: % del tiempo en cada color, horario medido, pedidos de ayuda, espera
  promedio hasta que la profesora atendió y el momento con más ruido.
- **Gráfico** del ruido cada 5 minutos con los pedidos de ayuda marcados en azul.
- **Relación ruido–ayuda**: para cada pedido se muestra cómo estuvo el ruido en los 10 minutos
  anteriores, y un resumen ("3 de 4 pedidos llegaron después de ruido alto").
- **Motivos y notas**: solo se ven con la clave de la profesora (son datos de los estudiantes).
  La profesora puede anotar o cambiar el motivo de un pedido desde la tabla.
- **Descargar (Excel)**: un archivo `.csv` del día con los pedidos y el ruido cada 5 minutos.

## API

| Ruta | Método | Quién la usa | Protección |
|------|--------|--------------|------------|
| `/api/salud` | GET | Prueba de conexión con la base | — |
| `/api/lecturas` | POST | ESP32: `{"dispositivo","sensor":"ruido","valor","nivel"}` | `x-api-key` |
| `/api/lecturas?limite=N` | GET | Página: últimas mediciones | — |
| `/api/estado` | GET | Página y ESP32: color actual, ayuda activa, resumen del día | — |
| `/api/asistencia` | POST | ESP32: un estudiante pidió ayuda | `x-api-key` |
| `/api/profe/entrar` | POST | Página: comprobar la clave de la profesora | `x-profe-clave` |
| `/api/asistencia/atender` | POST | Página: la profesora apaga la luz azul (`{"motivo","nota"}` opcional) | `x-profe-clave` |
| `/api/asistencia/:id/motivo` | POST | Registro: anotar o cambiar el motivo de un pedido | `x-profe-clave` |
| `/api/calendario?mes=AAAA-MM` | GET | Registro: resumen de cada día del mes | — |
| `/api/reporte?fecha=AAAA-MM-DD` | GET | Registro: reporte de un día (motivos solo con `x-profe-clave`) | — / `x-profe-clave` |

`nivel` puede ser `verde`, `amarillo` o `rojo`. `motivo` puede ser `actividad`, `sensorial`,
`emocion`, `companero`, `personal` u `otro`. Las tablas `lecturas` y `asistencias` (y sus
columnas nuevas) se crean solas al arrancar el servidor. Los días se cuentan en hora de Chile.

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
