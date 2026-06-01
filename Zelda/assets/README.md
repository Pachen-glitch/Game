# Assets

## Ejecutar el juego

Desde PowerShell:

```powershell
cd "c:\Users\Admin\Documents\Juego\Game\Zelda\build\Debug"
.\ZeldaASCII.exe
```

Si compilaste cambios recientes, primero reconstruye (cierra el `.exe` si está abierto):

```powershell
cd "c:\Users\Admin\Documents\Juego\Game\Zelda\build"
cmake --build . --config Debug
cd Debug
.\ZeldaASCII.exe
```

En el menú principal (imagen `menu/start.png`), haz clic en **Iniciar Juego** para empezar.

---

## Prueba rápida (sin descargar packs)

Desde PowerShell en la carpeta del proyecto:

```powershell
cd "c:\Users\Admin\Documents\Juego\Game\Zelda"
.\scripts\setup_test_assets.ps1
cmake --build build --config Debug
```

cd "c:\Users\Admin\Documents\Juego\Game\Zelda\build"
cmake --build . --config Debug
cd Debug
.\debug\ZeldaASCII.exe


Eso copia **Arial/Segoe** y sonidos del sistema Windows a `assets/`.

---

## Packs gratuitos recomendados (sprites + audio)

| Pack | Licencia | URL |
|------|----------|-----|
| **Kenney** (top-down, UI, audio) | CC0 | https://kenney.nl/assets |
| **Kenney Tiny Dungeon** | CC0 | https://kenney.nl/assets/tiny-dungeon |
| **OpenGameArt** | varias | https://opengameart.org/ |
| **Freesound** (SFX) | CC / atribución | https://freesound.org/ |

Kenney es ideal para prototipos: tilesets, personajes, monedas, UI y **Audio** en un solo zip.

---

## Estructura esperada por el motor

```
assets/
├── fonts/arial.ttf
├── sprites/
│   ├── player/move/walk_d1.png … walk_r5.png
│   ├── enemies/slime/, bat/, skeleton/, summoner/
│   ├── objects/coin.png, key.png, heart.png, door.png
│   └── npc/shopkeeper.png
└── audio/
    ├── music.ogg
    ├── attack.wav, pickup.wav, enemy_death.wav, hurt.wav
```

Si falta un PNG, verás **cuadrados magenta** (fallback). Si falta la fuente, el HUD usa la fuente de Windows automáticamente.

---

## Música

Cualquier `.ogg` en `assets/audio/music.ogg` (cuando reactivemos el módulo de audio SFML).
