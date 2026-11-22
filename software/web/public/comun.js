// comun.js — lo que usan las dos páginas (semáforo y registro diario)

const ZONA = "America/Santiago";
const COLORES = { verde: "#22c55e", amarillo: "#facc15", rojo: "#ef4444" };
const NOMBRES = { verde: "Tranquila", amarillo: "Subiendo", rojo: "Mucho ruido" };

// Motivos por los que un estudiante pide ayuda (las claves deben coincidir con server.js)
const MOTIVOS = {
  actividad: { emoji: "📚", texto: "Duda con la actividad" },
  sensorial: { emoji: "🎧", texto: "Mucho ruido o estímulos" },
  emocion:   { emoji: "💛", texto: "Emoción difícil" },
  companero: { emoji: "🤝", texto: "Problema con un compañero" },
  personal:  { emoji: "🚻", texto: "Necesidad personal" },
  otro:      { emoji: "✏️", texto: "Otro" },
};

const $ = id => document.getElementById(id);

function escapar(t) {
  return String(t ?? "").replace(/[&<>"']/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));
}
function hora(fecha, conSegundos = true) {
  const op = { hour: "2-digit", minute: "2-digit", hourCycle: "h23", timeZone: ZONA };
  if (conSegundos) op.second = "2-digit";
  return new Date(fecha).toLocaleTimeString("es-CL", op);
}
function formatearValor(v) {
  if (v === null || v === undefined) return "—";
  return Number.isInteger(v) ? v : Number(v).toFixed(1);
}
function hace(fecha) {
  const s = Math.max(0, Math.round((Date.now() - new Date(fecha).getTime()) / 1000));
  if (s < 10) return "recién";
  if (s < 60) return `hace ${s} segundos`;
  const m = Math.round(s / 60);
  if (m < 60) return m === 1 ? "hace 1 minuto" : `hace ${m} minutos`;
  const h = Math.round(m / 60);
  return h === 1 ? "hace 1 hora" : `hace ${h} horas`;
}
// Fecha de hoy en Chile como "AAAA-MM-DD"
function hoyEnChile() {
  return new Intl.DateTimeFormat("en-CA", { timeZone: ZONA, year: "numeric", month: "2-digit", day: "2-digit" }).format(new Date());
}

// ---------- Acceso de la profesora ----------
// La clave se guarda solo mientras la pestaña está abierta (sessionStorage).
const Profe = {
  clave: null,
  avisos: [],
  guardar(clave) {
    this.clave = clave || null;
    try {
      if (clave) sessionStorage.setItem("claveProfe", clave);
      else sessionStorage.removeItem("claveProfe");
    } catch (e) {}
    this.avisos.forEach(fn => fn(this.clave));
  },
  alCambiar(fn) { this.avisos.push(fn); fn(this.clave); },

  // Abre la ventana para escribir la clave. Devuelve true si entró.
  pedirClave() {
    return new Promise(resolve => {
      const d = crearDialogo("dialogo-profe", `
        <form method="dialog">
          <h2>Acceso profesora</h2>
          <p>Con tu clave puedes apagar la luz azul y ver los motivos de cada pedido de ayuda.</p>
          <label for="clave-profe" class="etiqueta">Clave</label>
          <input id="clave-profe" type="password" autocomplete="current-password" required>
          <p class="error" id="error-profe"></p>
          <div class="dialogo-botones">
            <button class="boton secundario" type="button" data-cancelar>Cancelar</button>
            <button class="boton azul" type="submit">Entrar</button>
          </div>
        </form>`);
      let entro = false;
      d.querySelector("[data-cancelar]").onclick = () => d.close();
      d.querySelector("form").onsubmit = async ev => {
        ev.preventDefault();
        const clave = $("clave-profe").value;
        try {
          const res = await fetch("/api/profe/entrar", { method: "POST", headers: { "x-profe-clave": clave } });
          if (!res.ok) { $("error-profe").textContent = "Clave incorrecta. Intenta de nuevo."; return; }
          this.guardar(clave);
          entro = true;
          d.close();
        } catch (e) {
          $("error-profe").textContent = "No se pudo conectar. Revisa el internet.";
        }
      };
      d.onclose = () => resolve(entro);
      d.showModal();
      $("clave-profe").focus();
    });
  },

  // POST con la clave. Si la clave dejó de servir, la pide de nuevo una vez.
  async enviar(url, cuerpo) {
    if (!this.clave && !(await this.pedirClave())) return null;
    const hacer = () => fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/json", "x-profe-clave": this.clave },
      body: JSON.stringify(cuerpo || {}),
    });
    let res = await hacer();
    if (res.status === 401) {
      this.guardar(null);
      if (!(await this.pedirClave())) return null;
      res = await hacer();
    }
    return res;
  },
};
try { Profe.clave = sessionStorage.getItem("claveProfe"); } catch (e) {}

// ---------- Ventana para elegir el motivo ----------
// Devuelve {motivo, nota} o null si se canceló. Con "sin motivo" devuelve {motivo: null, nota: null}.
function pedirMotivo({ titulo, texto, boton, motivo = null, nota = "", conSaltar = false }) {
  return new Promise(resolve => {
    const d = crearDialogo("dialogo-motivo", `
      <form method="dialog">
        <h2>${escapar(titulo)}</h2>
        <p>${escapar(texto)}</p>
        <div class="motivos" role="group" aria-label="Motivo">
          ${Object.entries(MOTIVOS).map(([k, m]) =>
            `<button type="button" class="motivo" data-motivo="${k}" aria-pressed="${k === motivo}"><span aria-hidden="true">${m.emoji}</span>${escapar(m.texto)}</button>`
          ).join("")}
        </div>
        <label for="nota-motivo" class="etiqueta">Nota (opcional)</label>
        <textarea id="nota-motivo" maxlength="300" placeholder="Ej: le molestaba el ruido del patio. Mejor sin nombres de estudiantes.">${escapar(nota || "")}</textarea>
        <p class="error" id="error-motivo"></p>
        <div class="dialogo-botones">
          ${conSaltar ? `<button class="boton secundario" type="button" data-saltar>Apagar sin anotar</button>` : `<button class="boton secundario" type="button" data-cancelar>Cancelar</button>`}
          <button class="boton azul" type="submit">${escapar(boton)}</button>
        </div>
      </form>`);
    let elegido = motivo;
    let resultado = null;
    d.querySelectorAll(".motivo").forEach(b => b.onclick = () => {
      elegido = b.dataset.motivo;
      d.querySelectorAll(".motivo").forEach(o => o.setAttribute("aria-pressed", o === b));
      $("error-motivo").textContent = "";
    });
    const saltar = d.querySelector("[data-saltar]");
    if (saltar) saltar.onclick = () => { resultado = { motivo: null, nota: null }; d.close(); };
    const cancelar = d.querySelector("[data-cancelar]");
    if (cancelar) cancelar.onclick = () => d.close();
    d.querySelector("form").onsubmit = ev => {
      ev.preventDefault();
      if (!elegido) { $("error-motivo").textContent = "Elige un motivo (o usa «Otro»)."; return; }
      resultado = { motivo: elegido, nota: $("nota-motivo").value.trim() || null };
      d.close();
    };
    d.onclose = () => resolve(resultado);
    d.showModal();
  });
}

// Crea (o reutiliza) un <dialog> con el contenido dado
function crearDialogo(id, html) {
  let d = $(id);
  if (!d) {
    d = document.createElement("dialog");
    d.id = id;
    document.body.appendChild(d);
  }
  d.innerHTML = html;
  return d;
}

function etiquetaMotivo(motivo) {
  const m = MOTIVOS[motivo];
  return m ? `<span class="motivo-tag">${m.emoji} ${escapar(m.texto)}</span>` : `<span class="motivo-tag vacio">Sin motivo</span>`;
}
