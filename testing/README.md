# Testing y validación

## Objetivo

Comprobar que el prototipo mide bien el ruido, que el pedido de ayuda llega y se atiende rápido, y que la información sirve a la profesora para cuidar el bienestar de los estudiantes, en especial de quienes tienen hipersensibilidad auditiva.

| Nivel | Qué se prueba | Estado |
|---|---|---|
| Técnico | Colores del semáforo, botón, envío de datos, apagado de la luz, comportamiento sin internet y seguridad | Una prueba de una hora hecha el 22-11-2026. Faltan las pruebas T1 a T8 completas |
| Usuarios | Al menos 5 personas usando el semáforo, el botón y la página | **Pendiente** |

## Carpetas

| Carpeta | Contenido |
|---|---|
| [reportes](reportes) | [Protocolo de pruebas](reportes/protocolo_pruebas.md). El reporte con resultados se agrega al terminar las pruebas |
| [datos](datos/README.md) | Datos reales exportados de la base de datos |
| [evidencias](evidencias) | Fotos y videos de las pruebas (pendiente) |

## Resultados hasta ahora

| Prueba | Resultado | Origen |
|---|---|---|
| Mediciones recibidas en una hora | 332, una cada 11.0 s en promedio | Real, 22-11-2026 |
| Cortes de más de 30 s | 4, el más largo de 141 s | Real, 22-11-2026 |
| Pedidos de ayuda registrados y apagados desde la página | 2 de 2 | Real, 22-11-2026 |
| Tiempo hasta que la profesora apagó la luz | 45 s y 126 s | Real, 22-11-2026 |
| Pruebas con usuarios | — | Pendiente |

## Análisis inicial

- El envío funciona, pero no es continuo: 4 cortes en una hora. Probablemente se deben a la señal del hotspot. En las pruebas en sala conviene dejar el celular cerca del semáforo y repetir la prueba T6.
- En esa hora la sala estuvo con ruido alto (amarillo o rojo) el 99 % del tiempo. Esto puede indicar que los umbrales del código con nube (25 / 50) son bajos para el lugar de la prueba. Hay que revisarlos en una sala real (prueba T1). **Después de las pruebas con usuarios, los umbrales finales quedaron en 12 / 20**, calibrados en la sala con el promedio de 10 s (ver [pruebas_usuarios.md](reportes/pruebas_usuarios.md)).
- Los dos pedidos de ayuda llegaron después de 10 minutos con ruido alto. Con solo dos casos no se puede concluir nada, pero muestra que el sistema ya puede calcular esa relación.
