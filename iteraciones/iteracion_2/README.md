# Iteración 2 - Avance #2

**Versión:** v2.0 · **Estado:** Prototipo Alpha IoT funcional

## Cambios respecto a la v1
**Fecha:** [22/10/2026]  

| Aspecto | v1 | v2 |
|---|---|---|
| Conectividad | ESP32 conectado al hotspot de un iPhone | ESP32 central crea su propia red WiFi |
| Botón del estudiante | — | ESP32-C3 SuperMini que envía la solicitud por WiFi |
| Luz de solicitud | — | RGB #2 en blanco, separada del semáforo |
| Monitoreo | Solo Serial Monitor | Página web en vivo (http://192.168.4.1) |
| Atención docente | — | Botón "Marcar como atendida" en la página |

## Problemas y soluciones

1. **Dependencia del hotspot de un celular:** el sistema dependía de un iPhone. Ahora el ESP32 crea su propia red (modo Access Point).
2. **Presiones del botón perdidas o lentas:** había que apretar varias veces. Las causas eran el ahorro de energía del WiFi, la antena débil de la placa mini y presiones mientras la placa estaba ocupada. Se desactivó el ahorro de energía, se ajustó la potencia de transmisión y el botón ahora usa una interrupción.
3. **El amarillo no se veía en el LED:** en la prueba con un solo LED (`software/src/pruebas/prueba_sensor_un_led`) el Serial Monitor indicaba el color correcto, pero el LED pasaba de verde a rojo sin mostrar amarillo. Se calibraron los umbrales observando la amplitud en el Serial Monitor: de 15 / 1000 se pasó a 17 / 42.

## Fotos del Prototipo v2



### Feedback del Avance #2

**Comentarios del profesor/ayudante:**
1. Achicar plano del boton y semaforo
   - Acción: se sacara la placa del boton y conectarmos solo los cables y para el semaforo prirorizaremos espacio para reducir espacio.

2. No esta subido a la red
   - Acción: Se planea descartar el wifi usado por ahora y conectar la placa a una red (hotspot de telefono) para asi guardar los datos.

### Version 3
## Próximos pasos (v3)
- Conectar el sistema a una red y guardar los datos en una base de datos para relacionarlos con días, bloques de clase, profesores y tener un backgorund de los alumnos.


