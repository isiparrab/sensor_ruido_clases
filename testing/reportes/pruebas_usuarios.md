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
| P4 | Estudiante | 20 |
| P5 | Profesora (simulada por una integrante del equipo) | 20 |

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
| P5 | Atender 3 pedidos de ayuda desde la página | Con la luz azul ya arreglada: al apretar el botón se encendió la **luz azul** en la caja y apareció el aviso en la página. La profesora eligió el motivo, escribió una nota y apagó la luz con "Guardar y apagar luz". Los 3 pedidos quedaron en el registro diario | ✅ Sí |

**Resumen:** 4 participantes en la sala + 1 prueba de rol de profesora. El envío de datos y del aviso a la nube funcionó en todas las pruebas. En las primeras pruebas fallaron dos cosas del prototipo físico (los umbrales del semáforo y la luz azul de ayuda); después de corregirlas, el sistema completo funcionó.

### Pedidos de ayuda registrados en la prueba de profesora (P5)

| Hora del pedido | Ruido 10 min antes | Atendido | Espera | Motivo | Nota de la profesora |
|---|---|---|---|---|---|
| 08:47 | Tranquila (14 % con ruido alto) | 08:48 | 1 min | Otro | "Alumno se sentía incómodo por el nivel de ruido en la sala de clases" |
| 08:50 | Tranquila (7 % con ruido alto) | 08:53 | 4 min | Necesidad personal | — |
| 08:58 | Tranquila (2 % con ruido alto) | 09:01 | 3 min | Otro | "Alumno tuvo problemas con uno de sus compañeros" |

**Espera promedio hasta ser atendido: 2,7 min** (3 pedidos, 3 atendidos).

## Fallas encontradas y qué hicimos

| Falla | Causa | Solución |
|---|---|---|
| El semáforo marcaba rojo con poco ruido | Los umbrales cargados en la placa eran demasiado bajos para el ruido real de la sala, distintos de los que usa la página | Se volvieron a cargar los umbrales calibrados en la sala y con P4 ya marcó verde con ruido bajo |
| La luz azul no se encendió al apretar el botón | El aviso sí llegó al ESP32 central (aparece en la página), así que la falla está en la conexión de la luz de ayuda (GPIO 13, pata B del LED RGB y su GND) | Se revisó la luz de ayuda y en la prueba de profesora (P5) ya se encendió en azul en los 3 pedidos |

## Qué aprendimos
- **Hay que calibrar en la sala real.** El ruido de fondo cambia según la sala, así que los umbrales deben ajustarse con el Serial Plotter en el lugar donde se va a usar el semáforo.
- **El semáforo físico y la página deben usar los mismos umbrales**, si no le muestran cosas distintas al curso y a la profesora.
- **El botón cumple su objetivo:** el estudiante pide ayuda sin hablar, la luz azul avisa en la sala y la profesora lo atiende desde la página. Los 3 pedidos se atendieron en 1 a 4 minutos.
- **El registro de motivos sirve:** uno de los pedidos fue justamente por incomodidad con el ruido, que es el problema que busca resolver el proyecto.
- **Próxima versión:** revisar el cableado antes de cada uso y que el ESP32 muestre por Serial cuando enciende la luz de ayuda, para encontrar fallas más rápido.

## Conexión con el problema
El problema del Avance 1 es que el ruido excesivo afecta a todo el curso, en especial a quienes tienen hipersensibilidad auditiva, y que no hay una forma de pedir ayuda en silencio. La prueba mostró que **el semáforo sirve para mostrar el ruido solo si está bien calibrado**: con los umbrales correctos marcó verde con poco ruido, como se esperaba. También mostró que **el botón permite pedir ayuda sin interrumpir la clase**: la luz azul se encendió, la profesora atendió los 3 pedidos en 2,7 minutos en promedio y quedó registrado por qué se pidió ayuda, por ejemplo, un estudiante incómodo por el nivel de ruido.

## Evidencias
Fotos en `testing/evidencias/`:
- `prueba_estudiantes_1.jpg` y `prueba_estudiantes_2.jpg`: participantes en la sala, con el semáforo en el escritorio y la página abierta.
- `prueba_boton_en_el_puesto.jpg`: participante apretando el botón desde su puesto.
- `prueba_pagina_web.jpg`: página web durante la prueba ("¡Qué buena sala!", nivel 17).
- `prueba_profesora_luz_azul_1.jpg` y `prueba_profesora_luz_azul_2.jpg`: luz azul encendida en la caja mientras la profesora anota el motivo en la página.
- `prueba_profesora_motivo.jpg`: ventana para elegir el motivo y escribir una nota antes de apagar la luz.
- `prueba_registro_pedidos_de_ayuda.jpg`: registro diario con los 3 pedidos, la hora, la espera y el motivo.
