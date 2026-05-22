# Lista de assets que usa el juego

Marca cada archivo cuando lo tengas. Tamaño sugerido: **48×48** (jugador/enemigos) o **32×32** (objetos).

## Fuente (UI — texto HUD, tienda, daño)

| Archivo | ¿Obligatorio? | Notas |
|---------|---------------|--------|
| `assets/fonts/arial.ttf` | Recomendado | O ejecuta `scripts/setup_test_assets.ps1` (copia de Windows) |

---

## Jugador — caminar (20 archivos)

Carpeta: `assets/sprites/player/move/`

| Archivo |
|---------|
| `walk_d1.png` `walk_d2.png` `walk_d3.png` `walk_d4.png` `walk_d5.png` |
| `walk_u1.png` `walk_u2.png` `walk_u3.png` `walk_u4.png` `walk_u5.png` |
| `walk_l1.png` `walk_l2.png` `walk_l3.png` `walk_l4.png` `walk_l5.png` |
| `walk_r1.png` `walk_r2.png` `walk_r3.png` `walk_r4.png` `walk_r5.png` |

- **d** = abajo, **u** = arriba, **l** = izquierda, **r** = derecha  
- Sin estos PNG: el jugador se ve **magenta** pero el juego funciona.

---

## Enemigos (4 archivos mínimo)

| Archivo |
|---------|
| `assets/sprites/enemies/slime/slime_idle.png` |
| `assets/sprites/enemies/bat/bat_idle.png` |
| `assets/sprites/enemies/skeleton/skeleton_idle.png` |
| `assets/sprites/enemies/summoner/summoner_idle.png` |

---

## Objetos (4 archivos)

| Archivo |
|---------|
| `assets/sprites/objects/coin.png` |
| `assets/sprites/objects/key.png` |
| `assets/sprites/objects/heart.png` |
| `assets/sprites/objects/door.png` |

---

## NPC (1 archivo)

| Archivo |
|---------|
| `assets/sprites/npc/shopkeeper.png` |

---

## Audio (opcional — audio desactivado por ahora)

Carpeta: `assets/audio/`

| Archivo | Uso |
|---------|-----|
| `attack.wav` | Espada |
| `pickup.wav` | Rupia / pickup |
| `hurt.wav` | Jugador dañado |
| `enemy_death.wav` | Enemigo muere |
| `music.ogg` | Música de fondo |

Ejecuta `scripts/setup_test_assets.ps1` para copiar WAV de prueba desde Windows.

---

## Resumen rápido

| Categoría | Cantidad |
|-----------|----------|
| Fuente | 1 |
| Animación jugador | 20 |
| Enemigos | 4 |
| Objetos | 4 |
| NPC | 1 |
| Audio | 5 |
| **Total sprites PNG** | **29** |

---

## Packs gratis para no dibujarlos a mano

- https://kenney.nl/assets/tiny-dungeon  
- https://kenney.nl/assets/top-down-shooter  
- https://kenney.nl/assets/digital-audio  

Licencia CC0 en Kenney — puedes usar y renombrar archivos a las rutas de arriba.
