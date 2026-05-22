# Asset checklist (estructura actual)

El motor resuelve rutas con `AssetPaths` — escanea carpetas reales y acepta nombres alternativos.

## Estructura esperada

```
assets/sprites/
├── player/move/     walk_d1.png … walk_r5.png
├── player/attack/
├── player/idle/
├── enemies/slime/
├── enemies/bat/
├── objects/hearts/
├── objects/keys/
├── objects/coins/
├── objects/door/    (singular)
├── map/doors/
└── ui/health/
```

## Al arrancar

Verás en consola:

```
[AssetPaths] assets root: ...
[AssetPaths] sprites indexed: N PNG(s)
[AssetPaths] Validating critical assets...
  [ok] player_walk_d1
  [ok] slime
  ...
```

Los faltantes se loguean **una sola vez** (sin spam por frame).

## Después de añadir PNGs

```powershell
cmake --build build --config Debug
```

CMake copia `Zelda/assets/` → `build/Debug/assets/`.
