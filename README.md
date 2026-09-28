### Información del Equipo
- **Integrantes:**
  - Matilda Steiner
  - Isidora Parra
  - Isidora Vidal
  - Florencia Gutiérrez
  
- **ODS Seleccionado:**
- Número 04: Educación de Calidad
- Número 10: Reducción de Desigualdades
- **Problema a resolver:** 
La falta de herramientas didácticas atractivas que ayuden a niños del Trastorno del Espectro Autista, ante crisis provocadas por desregulación del ruido ambiental que supere el umbral de tolerancia acústica del alumno.

### Descripción del Proyecto
La solución loT propuesta consiste en un sistema integrado de detección acústica y comunicación audio visual silenciosa, compuesto por dos unidades interconectadas; el módulo del estudiante y módulo principal de la pizarra. El sistema opera bajo una lógica de doble entrada (Híbrida: Automática + Manual), orientada a prevenir desregulaciones sin emitir ruidos adicionales: un módulo central ubicado junto a la pizarra utiliza un micrófono de precisión para medir en tiempo real el nivel de ruido del aula, filtrando sonidos esporádicos y  encendiendo automáticamente una luz tenue  de aviso en la pizarra cuando al intensidad; esta respetando el umbral (luz verde), esta pronto a superarlo (luz amarilla) y cuando se encuentra por sobre él (luz roja). En paralelo el estudiante dispone el su escritorio de un botón táctil ergonómico interconectado que le permite encender manualmente la misma luz de la pizarra en cualquier momento si experimenta sobrecarga o angustia sensorial antes de alcanzarse dicho umbral. Al activarse la alerta visual por cualquiera de las dos vías, el docente identifica de forma inmediata y silenciosa la necesidad de bajar el volumen o tono de voz del grupo e intervenir pedagógicamente, reanudandose el estado normal del indicador una vez que el ambiente retorna a los niveles de confort acústico.
### Estado del Proyecto
- **Versión actual:** v1.0
- **Última actualización:** 28/09/2026
- **Estado:** Prototipo inicial

---

##  Estructura del Repositorio
```
├── hardware/          # Circuitos, esquemas y BOM
├── software/          # Código Arduino y librerías
├── diseno_3d/        # Modelos Fusion 360 y renders
├── testing/          # Validación con usuarios
├── documentacion/    # Reportes y presentación
└── iteraciones/      # Historial de versiones
```

---

##  Inicio Rápido

### Requisitos Previos
- Arduino IDE 2.x
- Fusion 360
- [Otras herramientas necesarias]

### Instalación
1. Clonar el repositorio
2. Instalar librerías (ver `software/librerias/`)
3. Cargar código en ESP32/Arduino
4. [Pasos adicionales]

---

##  Checklist de Entrega

### Hardware ✓
- [ ] Esquema del circuito (Fritzing/Wokwi)
- [ ] BOM completo
- [ ] Fotos de alta resolución

### Software ✓
- [ ] Código comentado
- [ ] Librerías documentadas
- [ ] Manual de instalación

### Diseño 3D ✓
- [ ] Archivos Fusion 360 (.f3d) con TODOS los componentes
- [ ] Renders de alta calidad (1+ ángulos)
- [ ] Planos técnicos

### Testing ✓
- [ ] Reporte de testing con usuarios (mín. 5)
- [ ] Evidencias fotográficas/videos
- [ ] Datos cuantitativos

### Documentación ✓
- [ ] Reporte final (máx. 15 páginas)
- [ ] Presentación (10 min)

---


---

##  Licencia
Este proyecto es desarrollado como parte del curso TEI201 - Facultad de Ingenieria y Ciencias - Universidad Adolfo Ibañez
