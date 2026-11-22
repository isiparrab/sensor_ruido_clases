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
  console.log("Tablas 'lecturas' y 'asistencias' listas");
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
  if (!process.env.PROFE_CLAVE || req.get("x-profe-clave") !== process.env.PROFE_CLAVE) {
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

// La profesora apaga la luz azul
app.post("/api/asistencia/atender", revisarProfe, async (req, res) => {
  try {
    const r = await pool.query(
      "UPDATE asistencias SET atendida_en = NOW() WHERE atendida_en IS NULL"
    );
    res.json({ ok: true, atendidas: r.rowCount });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "Error al apagar la ayuda" });
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
