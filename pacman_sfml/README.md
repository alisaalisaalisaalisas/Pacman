![Pac-Man](pacman.png)

# Pac-Man SFML

A faithful Pac-Man recreation built with **C++17** and **SFML 2.6.2**. Features pixel-perfect rendering at 256×288 native resolution with clean scaling to any window size, authentic ghost AI behaviors, and full audio support.

---

## 📋 Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Building](#building)
- [Running](#running)
- [Controls](#controls)
- [Game Mechanics](#game-mechanics)
- [Asset System](#asset-system)

---

## Features

### Gameplay

- **Classic pellet system** — Small pellets for points, power pellets to frighten ghosts
- **Four unique ghosts** — Blinky, Pinky, Inky, and Clyde with distinct chase/scatter behaviors
- **Ghost AI states** — Scatter, Chase, Frightened, and Eaten modes with proper state transitions
- **Warp tunnels** — Teleport across the maze edges
- **Fruit bonus** — Timed bonus items for extra score
- **Lives & scoring** — 3 lives, persistent score and level tracking

### Technical

- **Pixel-perfect rendering** — 256×288 native buffer, integer-scaled to window
- **Sprite atlas system** — JSON-defined character sprites from BMP atlas
- **State machine** — Clean MainMenu → Playing → Paused → GameOver flow
- **Audio engine** — Background music with pellet, power-up, ghost-eat, and death sounds
- **Fullscreen toggle** — Seamless window/fullscreen switching
- **Controller support** — Keyboard + basic gamepad input

---

## Architecture

```text
┌─────────────────────────────────────────────────────────────┐
│                          Game                               │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌──────────────┐   │
│  │ Player  │  │  Ghost  │  │   Map   │  │ AudioManager │   │
│  │         │  │ ×4 AI   │  │ Tiles   │  │   Sounds     │   │
│  └─────────┘  └─────────┘  └─────────┘  └──────────────┘   │
│                      │                                      │
│              ┌───────┴───────┐                             │
│              │   Renderer    │                             │
│              │ ┌───────────┐ │                             │
│              │ │SpriteAtlas│ │                             │
│              │ │BitmapFont │ │                             │
│              │ └───────────┘ │                             │
│              └───────────────┘                             │
└─────────────────────────────────────────────────────────────┘
```

### Core Classes

| Class | Responsibility |
|-------|----------------|
| `Game` | Main loop, state machine, collision detection, game logic |
| `Player` | Pac-Man movement, animation, tile-based navigation |
| `Ghost` | Individual ghost AI with unique targeting per ghost type |
| `Map` | Tile grid, wall detection, warp tunnels, spawn points |
| `Renderer` | Native buffer rendering, sprite drawing, HUD, menus |
| `SpriteAtlas` | JSON-based sprite region lookup from texture atlas |
| `BitmapFont` | Custom bitmap font rendering |
| `AudioManager` | Music playback, sound effects |
| `Menu` | Menu system with mouse/keyboard navigation |

---

## Project Structure

```text
pacman_sfml/
├── src/                          # Source code (C++17)
│   ├── main.cpp                  # Entry point
│   ├── Game.cpp/h                # Core game loop & state machine
│   ├── Player.cpp/h              # Pac-Man entity
│   ├── Ghost.cpp/h               # Ghost AI (Blinky, Pinky, Inky, Clyde)
│   ├── Map.cpp/h                 # Tile map & warp tunnels
│   ├── Renderer.cpp/h            # Rendering pipeline
│   ├── SpriteAtlas.cpp/h         # Sprite region management
│   ├── BitmapFont.cpp/h          # Text rendering
│   ├── AudioManager.cpp/h        # Sound & music
│   ├── Menu.cpp/h                # Menu system
│   ├── Direction.h               # Direction enum & utilities
│   └── Types.h                   # TileCoord typedef
│
├── assets/
│   ├── sprites/
│   │   ├── atlas.bmp             # Character sprite sheet
│   │   ├── atlas.json            # Sprite region definitions
│   │   ├── tile.png              # Wall tile texture
│   │   ├── minicoin.png          # Small pellet
│   │   ├── bigcoin.png           # Power pellet
│   │   └── heart.png             # Life indicator
│   ├── fonts/
│   │   ├── PressStart2P.ttf      # Retro pixel font
│   │   └── BackG.jpg             # Background image
│   ├── sounds/
│   │   ├── music.wav             # Background loop
│   │   ├── eat.mp3               # Pellet eat sound
│   │   ├── death.wav             # Death sound
│   │   └── Pacman-death-sound.mp3
│   └── maps/
│       ├── level1.txt            # Main level layout
│       └── fallback.txt          # Backup level
│
├── build/vcpkg/                  # Build output (generated)
├── CMakeLists.txt                # CMake build configuration
├── CMakePresets.json             # CMake presets (vcpkg setup)
├── vcpkg.json                    # vcpkg dependency manifest
├── gen_atlas.py                  # Sprite atlas generator script
└── pacman.png                    # Project screenshot
```

---

## Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| **SFML** | 2.6.2 | Graphics, window, audio, input |
| **nlohmann-json** | latest | JSON parsing for sprite atlas |

### Build Tools Required

- **C++17 compiler** — GCC 15+ (MSYS2/UCRT64)
- **CMake** ≥ 3.21
- **Ninja** build system
- **vcpkg** for dependency management

---

## Building

### Quick Build (Already Configured)

If the project is already configured, just run:

```bash
cd build/vcpkg
ninja
```

### Initial Configuration

The project uses **CMake Presets** for configuration. Run from the project root:

```bash
cmake --preset vcpkg
```

This command:

- Uses the `vcpkg` preset from `CMakePresets.json`
- Sets Ninja as the generator
- Configures vcpkg toolchain from `$VCPKG_ROOT`
- Uses MSYS2/UCRT64 compilers
- Sets triplet to `x64-mingw-dynamic`
- Outputs to `build/vcpkg/`

Then build with:

```bash
ninja -C build/vcpkg
```

### Manual Configuration (Alternative)

If you need to configure manually without presets:

```bash
cmake -S . -B build/vcpkg -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe \
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe \
  -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic \
  -DVCPKG_HOST_TRIPLET=x64-mingw-dynamic

ninja -C build/vcpkg
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `PACMAN_COPY_ASSETS` | ON | Copy assets to output directory |
| `PACMAN_COPY_RUNTIME_DLLS` | ON | Copy DLLs on Windows |

> **Note:** On paths with non-ASCII characters, `objdump` may emit "Illegal byte sequence" warnings during linking — this is harmless.

---

## Running

Run `pacman.exe` from the vcpkg build directory:

```bash
cd build/vcpkg
./pacman.exe
```

The executable is named `pacman.exe` (not `pacman_sfml.exe`).

### Troubleshooting

- **Missing assets**: Ensure `assets/` folder is next to the executable
- **Black screen**: Check that font and sprite files loaded correctly
- **No audio**: Verify sound files exist in `assets/sounds/`

---

## Controls

| Action | Keys |
|--------|------|
| **Move** | Arrow keys or WASD |
| **Pause / Back** | Escape |
| **Fullscreen** | F11 |
| **Menu Select** | Enter or Space |
| **Menu Navigation** | Arrow keys or Mouse |

### Options Menu

- Use arrow keys to adjust volume
- Press Escape to return

---

## Game Mechanics

### Ghost Behaviors

Each ghost has a unique **scatter corner** and **chase target**:

| Ghost | Color | Behavior |
|-------|-------|----------|
| **Blinky** | Red | Chases Pac-Man directly |
| **Pinky** | Pink | Targets 4 tiles ahead of Pac-Man |
| **Inky** | Cyan | Complex targeting using Blinky's position |
| **Clyde** | Orange | Chases when far, scatters when close |

### Ghost Modes

```text
Scatter ←→ Chase (timed phases)
     ↓
Frightened (on power pellet)
     ↓
Eaten (returns to spawn)
```

### Scoring

- Small pellet: 10 points
- Power pellet: 50 points
- Frightened ghost: 200, 400, 800, 1600 (combo)
- Fruit bonus: varies by level

---

## Asset System

### Sprite Atlas

Sprites are defined in `atlas.json` with regions from `atlas.bmp`:

```json
{
  "pacman_right": { "x": 0, "y": 0, "w": 16, "h": 16 },
  "ghost_red": { "x": 16, "y": 0, "w": 16, "h": 16 }
}
```

### Map Format

Maps are text files (`level1.txt`) using characters:

| Char | Meaning |
|------|---------|
| `#` | Wall |
| `.` | Small pellet |
| `O` | Power pellet |
| `P` | Player spawn |
| `B/I/P/C` | Ghost spawns |
| `1-9` | Warp tunnel pairs |

---

## Configuration

### Native Resolution

The game renders at **256×288 pixels** (including 32px HUD), then integer-scales to fit the window. This ensures crisp pixel art at any size.

### Audio

- Background music loops during gameplay
- Sound effects for: eating, power-up activation, ghost consumption, death

---

## License

This project is provided as-is for educational purposes.

---

*Built with ❤️ using SFML*
