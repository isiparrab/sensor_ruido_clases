# Hardware

Esta carpeta documenta el hardware del prototipo: por qué se eligió cada componente, cómo se conecta y cómo se mantiene.

## Carpetas

| Carpeta | Contenido |
|---|---|
| [esquemas/](esquemas/README.md) | Circuito simulado en Wokwi |
| [bom/](bom/README.md) | Lista de materiales y conexiones ([BOM.csv](bom/BOM.csv)) |
| [fotos/](fotos/README.md) | Fotos del prototipo armado (pendiente) |

## Módulos

El sistema tiene dos unidades separadas para que el estudiante pueda pedir ayuda desde su puesto sin levantarse.

| Módulo | Dónde va | Qué tiene |
|---|---|---|
| Central (semáforo) | Junto a la pizarra | ESP32 DevKit, micrófono KY-038, LED RGB del semáforo y luz de ayuda azul |
| Botón del estudiante | En el escritorio | ESP32-C3 SuperMini y un pulsador |

## Componentes principales y por qué se eligieron

| Componente | Función | Justificación |
|---|---|---|
| ESP32 DevKit | Cerebro del módulo central | Tiene WiFi integrado y puede crear su propia red para el botón y conectarse a internet al mismo tiempo (modo `WIFI_AP_STA`). Tiene dos núcleos: uno mide el ruido y otro manda los datos sin congelar el semáforo. Su conversor ADC1 (GPIO 34) sigue funcionando con el WiFi encendido |
| Micrófono KY-038 | Mide el ruido de la sala | Es barato, fácil de conseguir y entrega una señal analógica (AO) que el ESP32 lee directo. Con la amplitud en una ventana de 50 ms se distingue bien una sala tranquila de una ruidosa. No mide decibeles calibrados, por eso los umbrales se ajustaron con pruebas |
| LED RGB de cátodo común | Semáforo | Un solo LED muestra los tres colores: verde, rojo, y amarillo al encender rojo y verde juntos. Ocupa menos espacio que tres LEDs y la luz es tenue, para no sobreestimular a los estudiantes |
| LED de aviso azul | Luz de ayuda | Separada del semáforo, para que la profesora distinga de inmediato un pedido de ayuda de un problema de ruido. El azul no se confunde con ningún color del semáforo |
| ESP32-C3 SuperMini | Cerebro del botón | Muy pequeña (22 × 18 mm), cabe en una carcasa de escritorio. Tiene WiFi para hablar con el central sin cables entre los puestos y un LED integrado (GPIO 8) que confirma al estudiante que su pedido llegó |
| Pulsador | Pedido de ayuda | Se conecta entre el GPIO 4 y GND usando la resistencia pull-up interna, sin componentes extra. Se lee con una interrupción para no perder ninguna presión |
| Hotspot de celular (2.4 GHz) | Internet para el central | El WiFi de la universidad pide usuario y contraseña, y el ESP32 no puede iniciar sesión en esas redes. El ESP32 solo funciona en 2.4 GHz |
| Cargadores USB de 5 V | Alimentación | Ambas placas se alimentan y programan por el mismo USB, sin fuentes adicionales |

## Conexiones

Los pines de cada versión del código están en [bom/README.md](bom/README.md). En la versión con nube (`central_nube.ino`), los pines del LED RGB se ajustaron al cableado real del prototipo (R 27, G 26, B 25) porque los colores salían cambiados.

## Diseño para la reparación

- **Componentes estándar y baratos.** ESP32, KY-038, LED RGB y pulsador se consiguen en cualquier tienda de electrónica y se reemplazan sin rediseñar nada.
- **Pines definidos al inicio del código.** Si se recablea o se cambia un LED, basta con cambiar las constantes `RGB1_R`, `RGB1_G`, `RGB1_B` y `LED_AVISO` y volver a subir el programa.
- **Programación por USB.** Las placas se actualizan con el mismo cable que las alimenta.
- **Módulos independientes.** Si falla el botón, el semáforo sigue funcionando. Si se cae el internet, el semáforo sigue midiendo y encendiendo el LED, y vuelve a mandar datos solo cuando la red regresa.
- **Los datos no viven en la placa.** Todo queda en la base de datos de la nube, así que cambiar un ESP32 dañado no borra el historial.
- **Monitor Serie para diagnosticar.** El central imprime la amplitud medida, si tiene internet y el resultado de cada envío. Ver la tabla de problemas comunes en [software/docs](../software/docs/README.md#problemas-comunes).

## Especificaciones

| Dato | Valor |
|---|---|
| Alimentación | 5 V por USB en cada módulo |
| Conectividad | WiFi 2.4 GHz: red propia del central para el botón e internet por hotspot |
| Dimensiones y peso | Pendiente, según el encapsulado final |
