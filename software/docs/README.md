# Documentación del software

| Documento | Contenido |
|---|---|
| [captura_y_datos.md](captura_y_datos.md) | Cómo se capturan los datos, dónde se guardan, el dashboard y la información que genera |
| [api.md](api.md) | Rutas del servidor, claves y ejemplos |
| [img/](img) | Capturas reales de la página publicada |

## Manual de operación

### Encendido

1. Encender el hotspot del celular (2.4 GHz) con el nombre y la clave cargados en el ESP32 central.
2. Conectar el ESP32 central a un cargador USB. El semáforo se enciende de inmediato, aunque todavía no haya internet.
3. Encender el botón del estudiante. Su LED parpadea mientras busca la red del central y se apaga cuando se conecta.
4. Abrir https://semaforoenlinea.up.railway.app. En menos de 15 s la pastilla de arriba debe decir **"Semáforo conectado"**.

### Durante la clase

- El semáforo de la pizarra y la página muestran el mismo color.
- Cuando un estudiante aprieta el botón, se enciende la luz de ayuda y la tarjeta de la página se pone azul.
- La profesora toca **"👩‍🏫 Acceso profesora"**, escribe su clave (queda guardada mientras la pestaña esté abierta), toca **"✓ Ya atendí"**, elige el motivo y la luz se apaga en unos 3 s.

### Después de la clase

- Abrir **"📅 Registro diario"**, elegir el día y revisar el reporte. Con la clave se ven los motivos.
- **"Descargar (Excel)"** baja los datos del día.
- Desconectar el ESP32. Los datos quedan guardados.

## Problemas comunes

| Problema | Causa probable | Solución |
|---|---|---|
| La página dice "Sin señal" | El ESP32 no tiene internet o está apagado | Revisar que el hotspot esté encendido y sea de 2.4 GHz. En el Monitor Serie debe decir `Internet: si` |
| `POST /api/lecturas -> 401` en el Monitor Serie | La `API_KEY` del `.ino` no es la misma que la de Railway | Copiar la clave exacta de Railway en el `.ino` y subirlo de nuevo |
| `-> -1` o `connection refused` | La URL del servidor está mal o el servidor está detenido | Revisar `SERVER_URL` (sin `/` al final) y que Railway diga *Online* |
| "Clave incorrecta" al entrar como profesora | La clave no coincide con `PROFE_CLAVE` en Railway | Pedir la clave al equipo; se cambia en *Variables* de Railway |
| El botón no enciende la luz | El botón no está conectado a la red del central | Ver la sección [Diferencias entre versiones](../src/README.md#diferencias-entre-versiones): la red y la ruta del botón deben coincidir con las del central |
| Los colores del LED salen cambiados | Las patas del LED RGB están en otros pines | Cambiar `RGB1_R`, `RGB1_G` y `RGB1_B` según el cableado |
