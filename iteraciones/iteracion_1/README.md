# Iteración 1, semáforo de ruido

**Versión:** v1 · **Fecha:** septiembre a octubre de 2026 (el equipo puede precisar la fecha) · **Estado:** prueba de concepto

## Qué se hizo

Un ESP32 DevKit leía el micrófono KY-038 y encendía un LED RGB en verde, amarillo o rojo según la amplitud del sonido. El ESP32 se conectaba al hotspot de un iPhone y los resultados se veían en el Monitor Serie.

## Componentes

- ESP32 DevKit
- Sensor de sonido KY-038 (AO → GPIO 34)
- LED RGB de cátodo común (GPIO 25, 26 y 27)
- Arduino IDE 2.x, sin librerías externas

## Qué se probó

Con el programa [prueba_sensor_un_led.ino](../../software/src/pruebas/prueba_sensor_un_led/prueba_sensor_un_led.ino) se comprobó que el KY-038 mide el ruido y que el LED cambia de color antes de agregar WiFi y botón.

## Problemas encontrados

1. **El amarillo no se veía.** El Monitor Serie indicaba el color correcto, pero el LED pasaba de verde a rojo muy rápido. Los umbrales de prueba (15 / 1000) no correspondían a las amplitudes reales de la sala.
2. **Dependía de un celular.** Sin el hotspot del iPhone el sistema no tenía red.
3. **No había forma de pedir ayuda.** El estudiante no tenía cómo avisar si se sentía sobrepasado antes de que el ruido llegara al rojo.

## Decisión

- Calibrar los umbrales observando la amplitud en el Monitor Serie (quedaron en 17 / 42).
- Que el ESP32 cree su propia red WiFi, para no depender de un celular.
- Agregar un botón en el escritorio del estudiante y una segunda luz en la pizarra.

Estos cambios forman la [iteración 2](../iteracion_2/README.md).

## Fotos

Pendiente: agregar fotos del prototipo v1 en esta carpeta.
