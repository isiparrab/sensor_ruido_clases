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
3. Encender el botón del estudiante (funciona con batería). Su LED parpadea en azul mientras busca la red del central y se apaga cuando se conecta.
4. Abrir https://semaforoenlinea.up.railway.app. En menos de 15 s la pastilla de arriba debe decir **"Semáforo conectado"**.

### Durante la clase

- El semáforo de la pizarra y la página muestran el mismo color.
- Cuando un estudiante aprieta el botón, su LED se pone verde (el aviso llegó), se enciende la luz de ayuda azul en el semáforo y la tarjeta de la página se pone azul. Si el LED del botón se pone rojo, el aviso no llegó y hay que volver a apretar.
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
| El LED del botón parpadea en azul sin parar | No encuentra la red `AulaRuido` | Revisar que el central esté encendido y cerca. La red y la clave del botón deben ser las mismas que en `central_nube.ino` |
| El LED del botón se pone rojo | El aviso no llegó al central | Volver a apretar. Si se repite, acercar el botón al central |
| Los colores del LED salen cambiados | Las patas del LED RGB están en otros pines | Cambiar `RGB1_R`, `RGB1_G` y `RGB1_B` según el cableado |
