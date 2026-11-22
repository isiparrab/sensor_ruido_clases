# Datos de testing

Datos reales exportados de la base de datos PostgreSQL del proyecto. No incluyen las pruebas hechas desde el computador (dispositivo `pc-prueba`), ni motivos ni notas de los pedidos de ayuda.

| Archivo | Contenido |
|---|---|
| [mediciones_esp32.csv](mediciones_esp32.csv) | Cada medición que mandó el ESP32: `id`, `dispositivo`, `fecha_hora_chile`, `valor` (amplitud promedio de 10 s) y `nivel` (verde, amarillo o rojo) |
| [pedidos_de_ayuda.csv](pedidos_de_ayuda.csv) | Cada pedido de ayuda: hora del pedido, hora en que la profesora lo atendió y segundos de espera |

## Prueba del 22-11-2026

| Dato | Valor |
|---|---|
| Horario | 08:00 a 09:01 |
| Mediciones | 332 |
| Verde / amarillo / rojo | 2 / 275 / 55 mediciones (1 % / 83 % / 17 %) |
| Amplitud promedio | 60.0 |
| Intervalo entre envíos | 11.0 s en promedio, 4 cortes de más de 30 s (el más largo de 141 s) |
| Pedidos de ayuda | 2, atendidos en 45 s y 126 s |

## Cómo agregar más datos

En https://semaforoenlinea.up.railway.app/registro.html, elegir el día y tocar **Descargar (Excel)**. Las respuestas de las pruebas con usuarios van en `usuarios.csv`, una fila por participante y sin nombres.
