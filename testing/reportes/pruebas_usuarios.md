# Prueba con usuarios — Semáforo de Ruido

**Fecha:** 23-11-2026 · **Lugar:** sala de clases, UAI · **Versión probada:** v3 (caja de MDF + botón a batería + página web en Railway)

## Objetivo
Comprobar que el sistema ayuda con el problema del Avance 1: que el semáforo muestre bien cuánto ruido hay en la sala, que un estudiante pueda **pedir ayuda en silencio** con el botón y que la profesora **vea y atienda** el aviso desde la página.

## Participantes
Identificados solo con un código (sin nombres).

| Código | Rol en la prueba | Edad |
|---|---|---|
| P1 | Estudiante | 20 |
| P2 | Estudiante | 21 |
| P3 | Estudiante | 19 |
| P4 | Estudiante | [edad] |
| P5 | Profesora (simulada por una integrante del equipo) | — |

## Tareas
1. **Semáforo:** los participantes conversan en voz normal y después en voz fuerte, y se observa el color del semáforo y lo que muestra la página web.
2. **Botón:** un participante aprieta el botón de su puesto para pedir ayuda sin hablar.
3. **Profesora (P5):** debe darse cuenta del aviso en la página web y apagarlo con el modo profesora.

## Resultados

| Código | Tarea | Qué pasó | ¿Funcionó? |
|---|---|---|---|
| P1, P2, P3 | Semáforo (voz normal y fuerte) | Con poco ruido el semáforo físico se veía **rojo**, mientras la página web mostraba que el ruido estaba bien (verde) | ❌ No: umbrales mal calibrados en la placa |
| P1, P2, P3 | Botón | El aviso **llegó a la página web**, pero la **luz azul** de la caja **no se encendió** | ⚠️ Parcial: llega a la nube, falla la luz física |
| P4 | Semáforo, después de recalibrar | Con los umbrales de vuelta en sus valores calibrados, con ruido bajo el semáforo se vio **verde**, igual que en la página | ✅ Sí |
| P5 | Atender el aviso desde la página | [completar después de la prueba] | [ ] |

**Resumen:** 4 participantes en la sala + 1 prueba de rol de profesora. El envío de datos y del aviso a la nube funcionó en todas las pruebas. Fallaron dos cosas del prototipo físico: los umbrales del semáforo y la luz azul de ayuda.

## Fallas encontradas y qué hicimos

| Falla | Causa | Solución |
|---|---|---|
| El semáforo marcaba rojo con poco ruido | Los umbrales cargados en la placa eran demasiado bajos para el ruido real de la sala, distintos de los que usa la página | Se volvieron a los umbrales calibrados ([amarillo] / [rojo]) y con P4 ya marcó verde con ruido bajo |
| La luz azul no se encendió al apretar el botón | El aviso sí llegó al ESP32 central (aparece en la página), así que la falla está en la conexión de la luz de ayuda (GPIO 13, pata B del LED RGB y su GND) | [completar: qué se revisó o corrigió] |

## Qué aprendimos
- **Hay que calibrar en la sala real.** El ruido de fondo cambia según la sala, así que los umbrales deben ajustarse con el Serial Plotter en el lugar donde se va a usar el semáforo.
- **El semáforo físico y la página deben usar los mismos umbrales**, si no le muestran cosas distintas al curso y a la profesora.
- **El botón cumple su objetivo:** el estudiante pide ayuda sin hablar y la profesora lo ve en la página, aunque la luz de la caja falló.
- **Próxima versión:** revisar el cableado antes de cada uso y que el ESP32 muestre por Serial cuando enciende la luz de ayuda, para encontrar fallas más rápido.

## Conexión con el problema
El problema del Avance 1 es que el ruido excesivo afecta a todo el curso, en especial a quienes tienen hipersensibilidad auditiva, y que no hay una forma de pedir ayuda en silencio. La prueba mostró que **el semáforo sirve para mostrar el ruido solo si está bien calibrado**: con los umbrales correctos marcó verde con poco ruido, como se esperaba. También mostró que **el botón permite pedir ayuda sin interrumpir la clase**, porque el aviso llegó a la página de la profesora.

## Evidencias
Fotos en `testing/evidencias/`:
- `prueba_estudiantes_1.jpg` y `prueba_estudiantes_2.jpg`: participantes en la sala, con el semáforo en el escritorio y la página abierta.
- `prueba_boton_en_el_puesto.jpg`: participante apretando el botón desde su puesto.
- `prueba_pagina_web.jpg`: página web durante la prueba ("¡Qué buena sala!", nivel 17).
