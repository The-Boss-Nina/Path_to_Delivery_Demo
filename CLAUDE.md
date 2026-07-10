# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
make        # Compile
make run    # Build and run
make clean  # Remove executable
```

**Dependencies**: SDL2, SDL2_image, SDL2_mixer, SDL2_ttf (via pkg-config)

```bash
# Install on Debian/Ubuntu
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

## Architecture

C++ delivery/driving game using SDL2 with a **state stack + component system**:

- **`Game`** (singleton) — SDL2 context, renderer, state stack (`pushState`/`popState`)
- **`State`** — abstract base; subclasses own a list of `GameObject*`
- **`GameObject`** — entity with position (`Rect`), angle, and a vector of `Component*`
- **`Component`** — abstract base; subclasses implement `Update(dt)` and `Render()`

### Game States

| State | Role |
|-------|------|
| `VehicleSelectState` | Menu: press 1–4 to pick vehicle, ESC to quit |
| `StageState` | Gameplay: loads map, spawns player, drives camera |

### Key Components

| Component | Role |
|-----------|------|
| `Vehicle` | Physics (speed, turning, friction, bounce), 8-directional animation, collision vs tilemap, HP |
| `DeliveryPlayer` | Reads `InputManager`, issues commands to `Vehicle`, configures per-vehicle stats |
| `TileMap` | Renders layers from CSV map file; also used for passability/collision checks |
| `SpriteRenderer` | Renders a `Sprite` with rotation |
| `Animator` | Sequences frames on a `Sprite` |
| `Collider` | Hitbox component used by `Collision` detection |

### Data Flow

```
InputManager → DeliveryPlayer → Vehicle (commands) → physics update → position
TileMap → Vehicle collision check → bounce / damage
Camera → follows player GameObject → clamps to map bounds
```

### Map Format (`recursos/map/cidade2.txt`)

```
width,height,depth,
<tile indices, comma-separated, row by row, layer by layer>
```

- Tile size: 128×128 px. Current map: 50×40 tiles (2 layers) = 6400×5120 px
- `-1` = empty/passable; `4`, `5`, `16` = road; other values = obstacles

### Vehicle Stats (set in `DeliveryPlayer.cpp`)

| Vehicle | Max Speed | Accel | Turn |
|---------|-----------|-------|------|
| Harley  | 360 px/s  | 220   | 130°/s |
| Vespa   | 320 px/s  | 180   | 150°/s |
| Uno     | 330 px/s  | 200   | 140°/s |
| Mustang | 380 px/s  | 240   | 110°/s |

### Input (gameplay)

`W`/`↑` Accelerate · `S`/`↓` Brake/Reverse · `A`/`←` Turn Left · `D`/`→` Turn Right · `ESC` Quit

### Resource Caching

`Resources` class (hash maps) caches SDL textures, music, and sound chunks by file path. Always load assets through `Resources::GetImage`, `Resources::GetMusic`, `Resources::GetSound`.