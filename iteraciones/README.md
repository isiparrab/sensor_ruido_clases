# Historial de iteraciones

Cada versión cuenta qué se probó, qué falló y qué se cambió.

```
iteraciones/
├── iteracion_1/    Semáforo de ruido con el ESP32 conectado al hotspot de un celular
├── iteracion_2/    Red WiFi propia, botón del estudiante y página local (Avance 2)
└── iteracion_3/    Conexión a la nube, luz azul, acceso de la profesora y registro diario
```

## Tabla comparativa

| Aspecto | v1 | v2 | v3 |
|---|---|---|---|
| Fecha | Septiembre a octubre de 2026 | 26-10-2026 (Avance 2) | 22-11-2026 |
| Sensor | KY-038 | KY-038 | KY-038 |
| Semáforo | LED RGB, umbrales sin calibrar (15 / 1000) | LED RGB, umbrales calibrados 17 / 42 | LED RGB con pines corregidos, umbrales 25 / 50 en el código con nube |
| Pedido de ayuda | — | Botón con ESP32-C3 y luz blanca (LED RGB #2) | Botón y luz azul, separada del semáforo |
| Conectividad | Hotspot de un Android | Red WiFi propia del ESP32 (sin internet) | Red propia para el botón **e** internet por hotspot al mismo tiempo |
| Dónde se ve la información | Monitor Serie | Página local en `192.168.4.1` (solo conectado a la red del ESP32) | Página pública en Railway, desde cualquier celular o computador |
| Quién apaga la luz de ayuda | — | Cualquiera, desde la página local o el botón | Solo la profesora, con su clave |
| Datos guardados | Ninguno | Ninguno (se pierden al apagar) | Todas las mediciones y pedidos en PostgreSQL, con calendario y reporte diario |
| Principal problema | El amarillo no se veía, mal calibrado | No guardaba datos y no estaba en la red | Si bien se pudo hacer la validación con usuarios reales, no son el público objetivo, los cuales son niños de básica principalmente |

## Incorporación del feedback

| Feedback del Avance 2 | Qué se hizo en la v3 |
|---|---|
| "No está subido a la red" | El ESP32 manda cada 10 s las mediciones y los pedidos de ayuda a un servidor en Railway con base de datos en Aiven. Ver [iteración 3](iteracion_3/README.md) |
| "Achicar el plano del botón y del semáforo" | Pendiente en el diseño 3D: sacar la placa del botón y dejar solo los cables, y priorizar espacio en el semáforo |

## Lecciones aprendidas

1. **Calibrar con datos reales, no con números supuestos.** Los umbrales iniciales (15 / 1000) hacían que el amarillo no apareciera nunca. Mirar la amplitud en el Monitor Serie permitió ajustarlos.
2. **El WiFi de placas pequeñas necesita cuidado.** Las presiones perdidas del botón se resolvieron con interrupciones y desactivando el ahorro de energía, no cambiando de placa.
3. **Guardar los datos cambia el valor del sistema.** Con el registro diario, el semáforo deja de ser solo una luz y pasa a responder preguntas del equipo docente, como si los pedidos de ayuda llegan después de momentos de ruido alto.
4. **Revisar el cableado contra el código.** Los colores cambiados del LED en la v3 venían de pines distintos a los del código, no de un error en el programa.

## Detalle por versión

- [Iteración 1](iteracion_1/README.md)
- [Iteración 2](iteracion_2/README.md)
- [Iteración 3](iteracion_3/README.md)
