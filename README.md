# Bas Veeg Arc 3D

A fully-featured 3D beat 'em up game built with C++, Ogre3D, Bullet Physics, and OpenAL.

## Overview

Bas Veeg Arc 3D is a complete reimagining of the original 2D game in stunning 3D graphics. Features 13 playable characters, 6 epic bosses, story mode, multiplayer, and high-quality PBR graphics.

## Features

### Characters
- **13 Playable Characters** with unique abilities:
  - **Bas** - AOE splash damage specialist
  - **Berkay** - Kebab-powered warrior
  - **Luca** - Winter arc mode fighter
  - **Gefferinho** - Balanced rage specialist
  - **Hadi** - Lightning-fast Dubai Emirates fighter
  - **Nitin** - Fire damage over time specialist
  - **PalaBaba (Yigit Baba)** - THE ULTIMATE FIGHTER
  - **Fufinho** - Projectile master
  - **Efe abi** - Lahmacun-powered warrior
  - **Jad** - KFC rage incarnate
  - **Umut** - Terraria arc builder
  - **Keizer Bom Taha** - Aerial bombardment commander
  - **Goon Lord Tobese** - Special Milk power user

### Game Modes
- **Story Mode** - Complete campaign with cutscenes and boss fights
- **Versus (Local)** - Local competitive multiplayer
- **Co-op (Local)** - Team up locally against enemies
- **Online Versus** - Competitive online play
- **Online Co-op** - Cooperative online missions

### Bosses
- **Bastiaan** (800 HP) - Artist-themed boss
- **Keizer Bom Taha** (900 HP) - Military commander (boss version)
- **Mees** (750 HP) - Pita sirracha thrower
- **Principal Van Der Berg** (1200 HP) - 3-phase authority figure
- **The Janitor King** (1500 HP) - Master of mop combat
- **Head Chef Ramsey** (1000 HP) - Culinary combat expert

### Graphics
- **PBR (Physically Based Rendering)** materials
- **Post-processing effects**: Bloom, HDR, SSAO, Motion Blur
- **Dynamic lighting** with cascaded shadow maps
- **Particle systems** for abilities and combat effects
- **High-quality graphics** settings (Low, Medium, High, Ultra)

### Audio
- **3D spatial audio** with OpenAL
- **Character voice lines** (Dutch with English subtitles)
- **Dynamic sound effects** for combat
- **Background music** system

### Physics
- **Bullet Physics** integration
- **Character controllers** with collision detection
- **Ragdoll physics** (planned)
- **Environmental destruction** (planned)

### Multiplayer
- **ENet** low-latency networking
- **Client-server architecture**
- **Lag compensation**
- **Local and online modes**

## Requirements

### System Requirements
- **OS**: Windows 10/11 or Linux (kernel 5.0+)
- **CPU**: Quad-core 2.5GHz or better
- **GPU**: NVIDIA GTX 960 / AMD R9 280 or better (OpenGL 4.5+)
- **RAM**: 8 GB minimum, 16 GB recommended
- **Storage**: 5 GB available space
- **Network**: Broadband internet for online play

### Build Dependencies
- C++20 compatible compiler (GCC 11+, Clang 13+, MSVC 2022+)
- CMake 3.20+
- Ogre3D 13.0+
- Bullet Physics 3.24+
- OpenAL
- ENet

## Building from Source

### Linux (Gentoo)

```bash
# Install dependencies (Gentoo)
emerge -av dev-games/ogre \
            sci-physics/bullet \
            media-libs/openal \
            net-libs/enet \
            dev-util/cmake

# Clone and build
git clone <repository-url>
cd bas-veeg-arc-2
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./bin/BasVeegArc3D
```

### Windows

```powershell
# Install dependencies via vcpkg
vcpkg install ogre:x64-windows bullet3:x64-windows openal-soft:x64-windows enet:x64-windows

# Build
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release

# Run
bin\Release\BasVeegArc3D.exe
```

## Controls

### Keyboard & Mouse
- **WASD** - Movement
- **Space** - Jump
- **Left Click** - Attack
- **Right Click** - Special Ability
- **ESC** - Pause Menu
- **Tab** - Character Switch (story mode)

### Gamepad
- **Left Stick** - Movement
- **A/Cross** - Jump
- **X/Square** - Attack
- **B/Circle** - Special Ability
- **Start** - Pause Menu

## Project Structure

```
bas-veeg-arc-2/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── LICENSE                 # License information
├── docs/                   # Documentation
├── include/                # Header files
│   ├── core/              # Core engine systems
│   ├── graphics/          # Rendering systems
│   ├── physics/           # Physics systems
│   ├── audio/             # Audio systems
│   ├── gameplay/          # Game logic
│   ├── network/           # Networking
│   └── ui/                # User interface
├── src/                    # Source files
│   ├── main.cpp           # Entry point
│   ├── core/              # Core implementations
│   ├── graphics/          # Graphics implementations
│   ├── physics/           # Physics implementations
│   ├── audio/             # Audio implementations
│   ├── gameplay/          # Gameplay implementations
│   ├── network/           # Network implementations
│   └── ui/                # UI implementations
├── assets/                 # Game assets
│   ├── models/            # 3D models
│   ├── textures/          # Texture files
│   ├── audio/             # Sound files
│   └── animations/        # Animation files
├── shaders/               # GLSL shaders
└── build/                 # Build output (generated)
```

## Development Roadmap

### Phase 1: Core Engine ✅
- [x] Project structure
- [x] CMake build system
- [x] Ogre3D integration
- [x] Bullet Physics integration
- [x] OpenAL integration
- [x] Basic engine loop

### Phase 2: Graphics Pipeline (In Progress)
- [ ] PBR shader implementation
- [ ] Post-processing effects
- [ ] Particle system
- [ ] Lighting system
- [ ] Shadow mapping

### Phase 3: Gameplay Systems
- [ ] Character controller
- [ ] Combat system
- [ ] Ability system
- [ ] AI system
- [ ] Boss mechanics

### Phase 4: Content Creation
- [ ] 3D character models (AI-generated + refined)
- [ ] Character animations
- [ ] Environment models
- [ ] Sound effects and voice lines
- [ ] Music tracks

### Phase 5: Multiplayer
- [ ] Network protocol
- [ ] Client-server logic
- [ ] Matchmaking
- [ ] Lag compensation

### Phase 6: Polish
- [ ] UI/UX refinement
- [ ] Performance optimization
- [ ] Bug fixes
- [ ] Playtesting and balancing

### Phase 7: Distribution
- [ ] Packaging for Windows
- [ ] Packaging for Linux
- [ ] Steam integration (optional)
- [ ] Marketing materials

## Contributing

This is currently a solo project, but contributions may be welcome in the future.

## License

[License information to be determined]

## Credits

### Development
- Game Design & Programming: [Your Name]
- Original 2D Game: Bas Veeg Arc (Rust/Macroquad)

### Technologies
- **Ogre3D** - 3D rendering engine
- **Bullet Physics** - Physics simulation
- **OpenAL** - 3D audio
- **ENet** - Networking library

### Special Thanks
- All the characters are based on the original bas-veeg-arc game
- Dutch dialogue and character names preserved from original

## Contact

[Contact information]

---

**Note**: This is a work in progress. The game is being built from the ground up as a complete, distribution-ready title. Development is ongoing and features are being added continuously.
