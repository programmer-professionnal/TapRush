# 🎮 TapRush - Clicker Game

**Versión: v0.0.8** | Juego en desarrollo activo

---

## 🚀 Cómo Jugar

**¡Doble click en `TapRush.html` y listos!**

No requiere instalación, compilaciones ni dependencias. Solo abre el archivo en tu navegador.

---

## 📋 Contenido del Juego

### Sistemas Implementados

| Sistema | Descripción |
|---------|-------------|
| **Energía** | 500 clicks manuales por día |
| **Racha Diaria** | Bonus x1 a x7 por jugar consecutivos |
| **Misiones Permanentes** | 20 misiones con recompensas |
| **Misiones Diarias** | 6 misiones que se actualizan diariamente |
| **Logros** | 25 logros por desbloquear |
| **Prestigio** | Multiplicador permanente (requiere 10M puntos) |
| **Eventos Aleatorios** | 2x, 3x, 5x puntos cada 30 segundos |
| **Eventos Especiales** | Lucky Hour, Mega Combo, Click Frenzy |
| **Partículas** | Efectos visuales al hacer clic |

### Mejoras (10)

| # | Nombre | Costo | Efecto |
|---|--------|-------|--------|
| 1 | Cursor | 100 | +1/click |
| 2 | Auto Clicker | 5K | +1/s |
| 3 | Grandma | 50K | +5/s |
| 4 | Farm | 750K | +20/s |
| 5 | Mine | 15M | +100/s |
| 6 | Factory | 300M | +500/s |
| 7 | Temple | 5B | +2,500/s |
| 8 | Magic Portal | 75B | +10,000/s |
| 9 | Quantum Core | 1T | +50,000/s |
| 10 | Alien Tech | 25T | +250,000/s |

*Los precios suben 1.6x por cada compra*

---

## 🕹️ Controles

- **Click** - Hacer clic en el estrella
- **F11** - Pantalla completa
- **ESC** - Guardar y salir
- Click en las pestañas para cambiar de panel

---

## 💾 Guardado

El juego guarda automáticamente en el navegador (localStorage).
No necesitas hacer nada manual.

---

## 🔧 Desarrollo

- Lenguaje: HTML5 + JavaScript
- Sin dependencias externas
- Un solo archivo

---

## 📝 Changelog Completo v0.0.8

### v0.0.8 - Migración HTML
- ✅ Juego reescrito 100% en HTML/JavaScript
- ✅ Un solo archivo: TapRush.html
- ✅ Sin instalación required
- ✅ Funciona en cualquier navegador
- ✅ Todas las funciones del juego

### Versiones Anteriores (C++)
- v0.0.7 - Corrección de ventana + raylib 6.0
- v0.0.6 - Innovaciones Adictivas (能量, racha, misiones, prestigio, eventos)
- v0.0.5 - Mejoras y misiones corregidas
- v0.0.4 - Sistema de misiones

---

## 📋 Notas de Desarrollo

⚠️ **ESTE JUEGO ESTÁ EN DESARROLLO ACTIVO**

Nuevas funciones son añadidas regularmente.
¡Sugerencias bienvenidas!

---

## 📥 Descarga

1. Ve a [Releases](../../releases)
2. Descarga `TapRush.html`
3. Abre en tu navegador

**Windows/Mac/Linux**: Solo necesitas un navegador moderno (Chrome, Firefox, Edge, Safari)

---

**TapRush** - 🎯 ¡Haz click, sube de nivel, dominates el juego!

The executable will be in `build/TapRush.exe`.

## How to Play

- Click the big button to earn points
- Buy upgrades from the right panel to earn automatic clicks per second
- Upgrades scale in cost

## Customization

Modify these files to customize:

- **`src/Game.cpp`** - Game logic, upgrades, UI
- **`src/Button.cpp`** - Button rendering
- **`include/Game.h`** - Game class definition

### Adding New Upgrades

In `Game.cpp`, find `LoadUpgrades()` and add new entries:

```cpp
{"Upgrade Name", cost, clicksPerSecond, count},
```

### Adding New Features

1. Add methods to `Game.h`
2. Implement in `Game.cpp`
3. Draw in the `Draw()` function

## Required Dependencies

- CMake 3.10+
- MinGW (GCC for Windows)
- raylib 5.0

## License

Apache 2.0
