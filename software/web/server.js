// server.js - Semaforo de ruido para la sala de clases
// API + pagina web. Se despliega en Railway y guarda los datos en PostgreSQL de Aiven.
//
// El ESP32:
//   - manda el nivel de ruido y el color del semaforo   -> POST /api/lecturas
//   - avisa cuando un estudiante pide ayuda (luz azul)   -> POST /api/asistencia
//   - pregunta si la ayuda sigue activa                   -> GET  /api/estado
// La profesora (con su clave) apaga la luz azul           -> POST /api/asistencia/atender

require("dotenv").config();
const express = require("express");
const path = require("path");
const { Pool } = require("pg");

const app = express();
app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));

const ZONA_HORARIA = "America/Santiago";
const NIVELES = ["verde", "amarillo", "rojo"];
const SEGUNDOS_SIN_SENAL = 60; // si no llegan datos en este tiempo, el semaforo se muestra "sin senal"

// ---------- Conexion a la base de datos (Aiven exige SSL) ----------
// Se quita "?sslmode=require" de la URI porque el SSL se configura abajo.
const pool = new Pool({
  connectionString: (process.env.DATABASE_URL || "").split("?")[0],
  ssl: process.env.DB_CA_CERT
    ? { ca: process.env.DB_CA_CERT.replace(/\\n/g, "\n") }
    : { rejectUnauthorized: false },
  max: 5,
});

async function crearTablas() {
  await pool.query(`
    CREATE TABLE IF NOT EXISTS lecturas (
      id SERIAL PRIMARY KEY,
      dispositivo VARCHAR(50) NOT NULL,
      sensor VARCHAR(50) NOT NULL,
      valor REAL NOT NULL,
      creado_en TIMESTAMPTZ DEFAULT NOW()
    )
  `);
  // Columna nueva para el color del semaforo (se agrega sola si la tabla ya existia)
  await pool.query(`ALTER TABLE lecturas ADD COLUMN IF NOT EXISTS nivel VARCHAR(10)`);

  await pool.query(`
    CREATE TABLE IF NOT EXISTS asistencias (
      id SERIAL PRIMARY KEY,
      dispositivo VARCHAR(50) NOT NULL,
      pedida_en TIMESTAMPTZ DEFAULT NOW(),
      atendida_en TIMESTAMPTZ
    )
  `);
  // Columnas nuevas: por qué pidió ayuda el estudiante (las anota la profesora)
  await pool.query(`ALTER TABLE asistencias ADD COLUMN IF NOT EXISTS motivo VARCHAR(20)`);
  await pool.query(`ALTER TABLE asistencias ADD COLUMN IF NOT EXISTS nota TEXT`);

  // Indices para que el registro diario busque rapido por fecha
  await pool.query(`CREATE INDEX IF NOT EXISTS lecturas_creado_en_idx ON lecturas (creado_en)`);
  await pool.query(`CREATE INDEX IF NOT EXISTS asistencias_pedida_en_idx ON asistencias (pedida_en)`);
  console.log("Tablas 'lecturas' y 'asistencias' listas");
}

// ---------- Motivos de los pedidos de ayuda ----------
// Los nombres que se ven en la pagina estan en public/comun.js
const MOTIVOS = ["actividad", "sensorial", "emocion", "companero", "personal", "otro"];

// Revisa {motivo, nota} del cuerpo. Devuelve {error} o {motivo, nota}
function leerMotivo(body) {
  const motivo = body && body.motivo ? String(body.motivo) : null;
  const nota = body && body.nota ? String(body.nota).trim().slice(0, 300) : null;
  if (motivo && !MOTIVOS.includes(motivo)) return { error: "Motivo no valido" };
  return { motivo, nota: nota || null };
}

function esProfe(req) {
  return !!process.env.PROFE_CLAVE && req.get("x-profe-clave") === process.env.PROFE_CLAVE;
}

// "2026-11-20" -> true si es una fecha valida
function fechaValida(texto) {
  if (!/^\d{4}-\d{2}-\d{2}$/.test(texto || "")) return false;
  const d = new Date(texto + "T12:00:00Z");
  return !isNaN(d) && d.toISOString().slice(0, 10) === texto;
}

// ---------- Seguridad ----------
// Clave compartida con el ESP32
function revisarClave(req, res, next) {
  if (!process.env.API_KEY || req.get("x-api-key") !== process.env.API_KEY) {
    return res.status(401).json({ error: "API key invalida" });
  }
  next();
}

// Clave de la profesora para apagar la luz azul
function revisarProfe(req, res, next) {
  if (!esProfe(req)) {
    return res.status(401).json({ error: "Clave de profesora incorrecta" });
  }
  next();
}

// ---------- Rutas ----------
app.get("/api/salud", async (req, res) => {
  try {
    await pool.query("SELECT 1");
    res.json({ ok: true, db: "conectada" });
  } catch (err) {
    res.status(500).json({ ok: false, error: err.message });
  }
});

// El ESP32 manda: {"dispositivo":"semaforo-sala","sensor":"ruido","valor":62,"nivel":"verde"}
app.post("/api/lecturas", revisarClave, async (req, res) => {
  const { dispositivo, sensor, valor, nivel } = req.body;
  if (!dispositivo || !sensor || typeof valor !== "number" || !Number.isFinite(valor)) {
    return res.status(400).json({ error: "Faltan campos o 'valor' no es numero" });
  }
  if (nivel !== undefined && !NIVELES.includes(nivel)) {
    return res.status(400).json({ error: "'nivel' debe ser verde, amarillo o rojo" });
  }
  try {
    const r = await pool.query(
      "INSERT INTO lecturas (dispositivo, sensor, valor, nivel) VALUES ($1, $2, $3, $4) RETURNING id",
      [dispositivo, sensor, valor, nivel || null]
    );
    res.status(201).json({ ok: true, id: r.rows[0].id });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al guardar" });
  }
});

// Ultimas lecturas (para el grafico y el registro)
app.get("/api/lecturas", async (req, res) => {
  const limite = Math.min(parseInt(req.query.limite) || 50, 500);
  try {
    const r = await pool.query(
      "SELECT * FROM lecturas ORDER BY id DESC LIMIT $1",
      [limite]
    );
    res.json(r.rows);
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al leer" });
  }
});

// Estado actual: lo usan la pagina y el ESP32 (para saber si apagar la luz azul)
app.get("/api/estado", async (req, res) => {
  try {
    const ultima = await pool.query(
      "SELECT valor, nivel, creado_en FROM lecturas WHERE nivel IS NOT NULL ORDER BY id DESC LIMIT 1"
    );
    const ayuda = await pool.query(
      "SELECT id, dispositivo, pedida_en FROM asistencias WHERE atendida_en IS NULL ORDER BY id ASC LIMIT 1"
    );
    const hoy = await pool.query(
      `SELECT nivel, COUNT(*)::int AS cantidad FROM lecturas
       WHERE nivel IS NOT NULL
         AND creado_en >= date_trunc('day', NOW() AT TIME ZONE $1) AT TIME ZONE $1
       GROUP BY nivel`,
      [ZONA_HORARIA]
    );
    const ayudasHoy = await pool.query(
      `SELECT COUNT(*)::int AS cantidad FROM asistencias
       WHERE pedida_en >= date_trunc('day', NOW() AT TIME ZONE $1) AT TIME ZONE $1`,
      [ZONA_HORARIA]
    );

    const lectura = ultima.rows[0] || null;
    const conectado = !!lectura &&
      (Date.now() - new Date(lectura.creado_en).getTime()) / 1000 < SEGUNDOS_SIN_SENAL;
    const resumen = { verde: 0, amarillo: 0, rojo: 0 };
    hoy.rows.forEach(f => { resumen[f.nivel] = f.cantidad; });

    res.json({
      nivel: lectura ? lectura.nivel : null,
      valor: lectura ? lectura.valor : null,
      actualizado_en: lectura ? lectura.creado_en : null,
      conectado,
      asistencia: ayuda.rows[0]
        ? { activa: true, desde: ayuda.rows[0].pedida_en }
        : { activa: false },
      hoy: { ...resumen, ayudas: ayudasHoy.rows[0].cantidad },
      ahora: new Date().toISOString(),
    });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al leer el estado" });
  }
});

// El ESP32 avisa que un estudiante pidio ayuda (boton -> luz azul)
app.post("/api/asistencia", revisarClave, async (req, res) => {
  const dispositivo = (req.body && req.body.dispositivo) || "semaforo";
  try {
    // Si ya hay una ayuda pendiente no se crea otra
    const pendiente = await pool.query(
      "SELECT id FROM asistencias WHERE atendida_en IS NULL LIMIT 1"
    );
    if (pendiente.rows[0]) {
      return res.json({ ok: true, id: pendiente.rows[0].id, yaEstabaActiva: true });
    }
    const r = await pool.query(
      "INSERT INTO asistencias (dispositivo) VALUES ($1) RETURNING id",
      [String(dispositivo).slice(0, 50)]
    );
    res.status(201).json({ ok: true, id: r.rows[0].id });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al guardar la ayuda" });
  }
});

// La profesora comprueba su clave al entrar
app.post("/api/profe/entrar", revisarProfe, (req, res) => {
  res.json({ ok: true });
});

// La profesora apaga la luz azul y (si quiere) anota por que pidio ayuda
// Cuerpo opcional: {"motivo":"sensorial","nota":"le molestaba el ruido del patio"}
app.post("/api/asistencia/atender", revisarProfe, async (req, res) => {
  const m = leerMotivo(req.body);
  if (m.error) return res.status(400).json({ error: m.error });
  try {
    const r = await pool.query(
      `UPDATE asistencias SET atendida_en = NOW(), motivo = $1, nota = $2
       WHERE atendida_en IS NULL`,
      [m.motivo, m.nota]
    );
    res.json({ ok: true, atendidas: r.rowCount });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al apagar la ayuda" });
  }
});

// La profesora agrega o cambia el motivo de un pedido ya atendido (desde el registro)
app.post("/api/asistencia/:id/motivo", revisarProfe, async (req, res) => {
  const id = parseInt(req.params.id, 10);
  const m = leerMotivo(req.body);
  if (!Number.isInteger(id)) return res.status(400).json({ error: "id no valido" });
  if (m.error) return res.status(400).json({ error: m.error });
  try {
    const r = await pool.query(
      "UPDATE asistencias SET motivo = $1, nota = $2 WHERE id = $3",
      [m.motivo, m.nota, id]
    );
    if (!r.rowCount) return res.status(404).json({ error: "No existe ese pedido" });
    res.json({ ok: true });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al guardar el motivo" });
  }
});

// ---------- Registro diario ----------
// Rango [inicio, fin) de un dia o mes en hora de Chile, como timestamptz
const DESDE = "($2::date)::timestamp AT TIME ZONE $1";

// Calendario: un resumen por dia del mes. GET /api/calendario?mes=2026-11
app.get("/api/calendario", async (req, res) => {
  const mes = String(req.query.mes || "");
  if (!/^\d{4}-(0[1-9]|1[0-2])$/.test(mes)) {
    return res.status(400).json({ error: "Usa ?mes=AAAA-MM" });
  }
  const params = [ZONA_HORARIA, mes + "-01"];
  const HASTA = "(($2::date + interval '1 month'))::timestamp AT TIME ZONE $1";
  try {
    const lecturas = await pool.query(
      `SELECT to_char(creado_en AT TIME ZONE $1, 'YYYY-MM-DD') AS fecha, nivel, COUNT(*)::int AS cantidad
       FROM lecturas
       WHERE nivel IS NOT NULL AND creado_en >= ${DESDE} AND creado_en < ${HASTA}
       GROUP BY 1, 2`,
      params
    );
    const ayudas = await pool.query(
      `SELECT to_char(pedida_en AT TIME ZONE $1, 'YYYY-MM-DD') AS fecha, COUNT(*)::int AS cantidad
       FROM asistencias
       WHERE pedida_en >= ${DESDE} AND pedida_en < ${HASTA}
       GROUP BY 1`,
      params
    );
    const dias = {};
    const dia = f => (dias[f] = dias[f] || { fecha: f, verde: 0, amarillo: 0, rojo: 0, ayudas: 0 });
    lecturas.rows.forEach(f => { dia(f.fecha)[f.nivel] = f.cantidad; });
    ayudas.rows.forEach(f => { dia(f.fecha).ayudas = f.cantidad; });
    res.json({ mes, dias: Object.values(dias).sort((a, b) => a.fecha.localeCompare(b.fecha)) });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al leer el calendario" });
  }
});

// Reporte de un dia. GET /api/reporte?fecha=2026-11-20
// Los motivos y notas solo se entregan si viene la clave de la profesora.
app.get("/api/reporte", async (req, res) => {
  const fecha = String(req.query.fecha || "");
  if (!fechaValida(fecha)) return res.status(400).json({ error: "Usa ?fecha=AAAA-MM-DD" });
  const params = [ZONA_HORARIA, fecha];
  const HASTA = "(($2::date + 1))::timestamp AT TIME ZONE $1";
  const profe = esProfe(req);
  try {
    const total = await pool.query(
      `SELECT COUNT(*)::int AS mediciones,
              COUNT(*) FILTER (WHERE nivel = 'verde')::int AS verde,
              COUNT(*) FILTER (WHERE nivel = 'amarillo')::int AS amarillo,
              COUNT(*) FILTER (WHERE nivel = 'rojo')::int AS rojo,
              AVG(valor)::float AS promedio, MAX(valor)::float AS maximo,
              MIN(creado_en) AS primera, MAX(creado_en) AS ultima
       FROM lecturas
       WHERE nivel IS NOT NULL AND creado_en >= ${DESDE} AND creado_en < ${HASTA}`,
      params
    );
    // El dia en bloques de 5 minutos (para el grafico)
    const bloques = await pool.query(
      `SELECT to_char(b, 'HH24:MI') AS hora, promedio, maximo, verde, amarillo, rojo FROM (
         SELECT date_trunc('hour', creado_en AT TIME ZONE $1)
                  + floor(extract(minute FROM creado_en AT TIME ZONE $1) / 5) * interval '5 minutes' AS b,
                AVG(valor)::float AS promedio, MAX(valor)::float AS maximo,
                COUNT(*) FILTER (WHERE nivel = 'verde')::int AS verde,
                COUNT(*) FILTER (WHERE nivel = 'amarillo')::int AS amarillo,
                COUNT(*) FILTER (WHERE nivel = 'rojo')::int AS rojo
         FROM lecturas
         WHERE nivel IS NOT NULL AND creado_en >= ${DESDE} AND creado_en < ${HASTA}
         GROUP BY 1
       ) t ORDER BY b`,
      params
    );
    // Pedidos de ayuda + como estaba el ruido en los 10 minutos anteriores
    const pedidos = await pool.query(
      `SELECT a.id, a.pedida_en, a.atendida_en, a.motivo, a.nota,
              r.promedio, COALESCE(r.verde, 0) AS verde, COALESCE(r.amarillo, 0) AS amarillo, COALESCE(r.rojo, 0) AS rojo
       FROM asistencias a
       LEFT JOIN LATERAL (
         SELECT AVG(valor)::float AS promedio,
                COUNT(*) FILTER (WHERE nivel = 'verde')::int AS verde,
                COUNT(*) FILTER (WHERE nivel = 'amarillo')::int AS amarillo,
                COUNT(*) FILTER (WHERE nivel = 'rojo')::int AS rojo
         FROM lecturas l
         WHERE l.nivel IS NOT NULL
           AND l.creado_en > a.pedida_en - interval '10 minutes' AND l.creado_en <= a.pedida_en
       ) r ON true
       WHERE a.pedida_en >= ${DESDE} AND a.pedida_en < ${HASTA}
       ORDER BY a.pedida_en`,
      params
    );

    res.json({
      fecha,
      esProfe: profe,
      resumen: total.rows[0],
      bloques: bloques.rows,
      pedidos: pedidos.rows.map(p => ({
        id: p.id,
        pedida_en: p.pedida_en,
        atendida_en: p.atendida_en,
        ruidoAntes: { promedio: p.promedio, verde: p.verde, amarillo: p.amarillo, rojo: p.rojo },
        // Sin la clave de la profesora solo se sabe si tiene motivo, no cual
        motivo: profe ? p.motivo : undefined,
        nota: profe ? p.nota : undefined,
        tieneMotivo: !!p.motivo,
      })),
    });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al leer el reporte" });
  }
});

// ---------- Arranque ----------
const PORT = process.env.PORT || 3000; // Railway define PORT solo
app.listen(PORT, async () => {
  console.log(`Servidor escuchando en el puerto ${PORT}`);
  if (!process.env.PROFE_CLAVE) {
    console.warn("Aviso: falta la variable PROFE_CLAVE, nadie podra apagar la luz azul desde la pagina");
  }
  try {
    await crearTablas();
  } catch (err) {
    console.error("No se pudo conectar a la base de datos:", err.message);
  }
});
