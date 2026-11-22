# Protocolo de pruebas

Este protocolo define qué se prueba, cómo y qué cuenta como éxito. Tiene dos partes: **pruebas técnicas**, que el equipo puede hacer solo, y **pruebas con usuarios**, con al menos 5 personas reales.

> Borrador para que el equipo lo revise y ajuste antes de aplicarlo. Los resultados se anotan en `reporte_testing.md` y los datos en [../datos](../datos).

## Parte A, pruebas técnicas

Muchas métricas las mide el propio sistema: cada medición y cada pedido de ayuda quedan con fecha y hora en la base de datos, y se descargan desde el registro diario (**Descargar (Excel)**).

| Id | Qué se prueba | Cómo | Éxito si | Dato |
|---|---|---|---|---|
| T1 | El color corresponde al ruido | Con la sala en silencio, conversación normal y aplausos o música fuerte, anotar 10 veces el color del LED en cada caso | El color esperado aparece en al menos 9 de 10 intentos por caso | Tabla manual |
| T2 | El LED y la página muestran lo mismo | Comparar el color del LED con la página en 20 momentos al azar | Coinciden en al menos 18 de 20 (la página usa el promedio de 10 s, así que puede ir algo atrasada) | Tabla manual |
| T3 | El botón enciende la luz de ayuda | Apretar el botón 20 veces, separadas, y cronometrar hasta que se enciende la luz | 20 de 20 encendidas, en menos de 2 s | Tabla manual |
| T4 | El pedido llega a la página | En las mismas 20 presiones, cronometrar hasta que la tarjeta de la página se pone azul | 20 de 20, en menos de 10 s | Hora en `pedidos_de_ayuda.csv` |
| T5 | La profesora apaga la luz | Tocar "Ya atendí" y cronometrar hasta que se apaga el LED azul | 20 de 20, en menos de 6 s | Tabla manual |
| T6 | Continuidad de los datos | Dejar el sistema encendido una clase completa | Al menos el 95 % de los envíos esperados (uno cada 10 s) y ningún corte mayor a 60 s | `mediciones_esp32.csv` |
| T7 | Comportamiento sin internet | Apagar el hotspot 2 minutos y volver a encenderlo | El semáforo sigue funcionando, la página muestra "Sin señal" y los datos vuelven solos | Observación y CSV |
| T8 | Seguridad | Intentar apagar la luz con una clave equivocada y enviar datos sin `API_KEY` | Se rechazan (error 401) | Respuesta del servidor |

**Resultado ya medido (22-11-2026):** en una hora llegaron 332 mediciones (intervalo promedio de 11.0 s) con 4 cortes de más de 30 s, el más largo de 141 s. Por eso **T6 todavía no se cumple**, y conviene revisar la señal del hotspot durante la prueba.

## Parte B, pruebas con usuarios

### Participantes (mínimo 5)

| Perfil | Por qué | Cantidad sugerida |
|---|---|---|
| Profesoras o educadoras diferenciales | Son quienes usan la página y atienden los pedidos | 2 o más |
| Personas que hagan de estudiante | Prueban el botón y si entienden el semáforo | 2 o más |
| Persona con experiencia en TEA (terapeuta, apoderado o profesional) | Valida si la luz y los colores son adecuados y no sobreestimulan | 1 o más |

Si participan niños, debe ser con autorización de su apoderado y del colegio, sin registrar sus rostros ni nombres.

### Pasos de cada sesión (unos 20 minutos)

1. **Explicación (3 min).** Qué es el semáforo y para qué sirve, sin mostrar cómo se usa la página.
2. **Tareas (10 min).** El participante intenta cada tarea solo. Se anota si la logra, cuánto tarda y dónde duda.

   | Tarea | Para | Éxito si |
   |---|---|---|
   | Decir qué significa cada color del semáforo | Todos | Lo dice sin ayuda |
   | Pedir ayuda con el botón | Estudiantes | Lo hace en menos de 10 s y entiende que el pedido llegó |
   | Entrar como profesora, apagar la luz y elegir un motivo | Profesoras | Lo hace en menos de 1 minuto |
   | Encontrar cuántos pedidos de ayuda hubo un día anterior y a qué hora hubo más ruido | Profesoras | Lo encuentra en menos de 2 minutos |

3. **Encuesta (5 min).** Escala de 1 (muy en desacuerdo) a 5 (muy de acuerdo):
   - El semáforo se entiende sin explicación.
   - La luz no molesta ni distrae.
   - Pedir ayuda con el botón es fácil y discreto.
   - La página es fácil de usar.
   - El registro diario me ayudaría a entender al curso.
   - Usaría este sistema en una sala real.
4. **Preguntas abiertas (2 min).** ¿Qué te gustó? ¿Qué cambiarías? ¿Qué le falta?

### Qué se registra

- Una fila por participante en `testing/datos/usuarios.csv`: perfil, tareas logradas, tiempos, respuestas de la encuesta y comentarios. Sin nombres.
- Fotos o video de cada sesión en `testing/evidencias`, con consentimiento y sin mostrar rostros si no se autoriza.

## Validación contra el problema

El problema es la desregulación de niños con TEA por ruido sobre su umbral de tolerancia. El prototipo se considera validado si:

1. La profesora nota el aumento de ruido o el pedido de ayuda **sin que nadie tenga que hablar** (tareas y encuesta).
2. El pedido de ayuda llega y se atiende en poco tiempo (T3, T4, T5 y la espera promedio del registro).
3. El equipo docente puede usar el registro para relacionar el ruido con los pedidos de ayuda (última tarea y encuesta).
