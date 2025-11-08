# Development Guide - Bas Veeg Arc 3D

## Architecture Overview

### Engine Architecture

```
┌─────────────────────────────────────────┐
│           Main Engine Loop              │
│  (Fixed timestep physics & game logic)  │
│  (Variable timestep rendering)          │
└─────────────────────────────────────────┘
                    │
        ┌───────────┴────────────┐
        │                        │
    ┌───▼────┐             ┌─────▼──────┐
    │ Update │             │   Render   │
    └───┬────┘             └─────┬──────┘
        │                        │
┌───────┴─────────┐      ┌───────┴────────┐
│   Input (60Hz)  │      │ Graphics (VSync)│
│   Physics       │      │ Particles       │
│   Game Logic    │      │ Post-process    │
│   Network       │      │ UI Rendering    │
│   Audio         │      └─────────────────┘
└─────────────────┘
```

### Component Systems

#### Graphics Pipeline
1. **Scene Management** (Ogre3D)
   - Scene graph with hierarchical transforms
   - Entity-component architecture
   - Frustum culling

2. **Rendering Pipeline**
   - Forward rendering with PBR materials
   - Deferred lighting for many lights
   - Shadow mapping (cascaded for directional light)

3. **Post-Processing Stack**
   - Bloom (HDR glow)
   - Tone mapping (HDR to LDR)
   - SSAO (Screen-space ambient occlusion)
   - Motion blur
   - Anti-aliasing (FXAA/MSAA)

4. **Particle Systems**
   - GPU particle systems for effects
   - Ability-specific particle templates
   - Dynamic particle pooling

#### Physics System
1. **Bullet Physics Integration**
   - Discrete dynamics world
   - Character controllers (capsule-based)
   - Collision detection
   - Raycasting for attacks

2. **Character Physics**
   - Kinematic character controller
   - Ground detection
   - Slope handling
   - Jump mechanics

#### Audio System
1. **OpenAL 3D Audio**
   - Spatial audio with HRTF
   - Doppler effect
   - Distance attenuation
   - Reverb zones

2. **Audio Management**
   - Sound pooling
   - Priority system
   - Volume mixing (Master, SFX, Music, Voice)

#### Networking
1. **ENet Protocol**
   - Reliable and unreliable channels
   - Client-server architecture
   - State synchronization

2. **Lag Compensation**
   - Client-side prediction
   - Server reconciliation
   - Interpolation

## Coding Standards

### C++ Style Guide
- **Standard**: C++20
- **Naming Conventions**:
  - Classes: `PascalCase`
  - Functions: `camelCase`
  - Variables: `camelCase`
  - Constants: `UPPER_SNAKE_CASE`
  - Private members: `camelCase` (no prefix)

### Example:
```cpp
class CharacterController {
public:
    void updateMovement(float deltaTime);
    float getHealth() const { return currentHealth; }

private:
    float currentHealth;
    static constexpr float MAX_HEALTH = 100.0f;
};
```

### File Organization
- **Header files**: `include/category/ClassName.hpp`
- **Source files**: `src/category/ClassName.cpp`
- One class per file (generally)
- Forward declarations to minimize includes

### Memory Management
- Use RAII principles
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- No raw `new`/`delete` in application code
- Resource managers handle lifetime

## Building the Project

### Debug Build
```bash
./build.sh debug
```
- Compiler optimizations: `-O0`
- Debug symbols: `-g`
- Assertions enabled
- Slower but easier to debug

### Release Build
```bash
./build.sh release
```
- Compiler optimizations: `-O3 -march=native`
- No debug symbols
- Assertions disabled
- Fast performance

### Clean Build
```bash
./build.sh clean
./build.sh release
```

## Creating New Characters

### Step 1: Define Character in Header
```cpp
// include/gameplay/Characters.hpp
class MyCharacter : public Character {
public:
    MyCharacter() : Character(CharacterID::MyCharacter) {
        name = "My Character";
        ability = {
            "My Ability",
            "Voice line here!",
            {{AbilityEffect::DamageBoost, 2.0f}},
            5.0f,  // duration
            10.0f, // cooldown
            "Description here"
        };
        stats.maxHealth = 100.0f;
        stats.attackDamage = 10.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};
```

### Step 2: Implement Ability
```cpp
// src/gameplay/Characters.cpp
void MyCharacter::onAbilityActivated() {
    applyDamageBoost(2.0f, 5.0f);
    // Add visual effects, sounds, etc.
}
```

### Step 3: Add to Factory
```cpp
// src/gameplay/Character.cpp
std::unique_ptr<Character> createCharacter(CharacterID id) {
    switch (id) {
        case CharacterID::MyCharacter:
            return std::make_unique<MyCharacter>();
        // ... other characters
    }
}
```

## Creating 3D Models (AI-Assisted Workflow)

### Recommended Tools
1. **AI Generation**:
   - DALL-E 3 / Midjourney for concept art
   - Rodin / Luma AI for 3D model generation
   - CSM (Common Sense Machines) for character generation

2. **Refinement**:
   - Blender for topology cleanup
   - Substance Painter for PBR textures
   - Mixamo for rigging and animation

### Workflow
1. Generate concept art with AI
2. Generate base 3D model with AI
3. Import to Blender
4. Retopologize for game use (target: 10k-20k tris)
5. UV unwrap
6. Export to Ogre format (.mesh)
7. Create PBR textures (albedo, normal, metallic, roughness)
8. Rig skeleton
9. Animate (idle, walk, run, attack, ability, hit, death)

### Ogre Model Export
```bash
# Blender → Ogre exporter
# Install blender2ogre addon
# Export as .mesh with .skeleton and .material
```

## Testing

### Unit Tests (Planned)
```bash
cd build
ctest
```

### Performance Profiling
```bash
# Linux
perf record ./bin/BasVeegArc3D
perf report

# Or use Valgrind
valgrind --tool=callgrind ./bin/BasVeegArc3D
```

## Debugging

### GDB (Linux)
```bash
cd build
gdb ./bin/BasVeegArc3D
(gdb) run
(gdb) bt  # backtrace on crash
```

### Visual Studio (Windows)
- Open solution in Visual Studio
- Set breakpoints
- F5 to debug

## Common Issues

### Issue: Ogre3D not found
**Solution**: Install Ogre3D development package
```bash
# Gentoo
emerge -av dev-games/ogre

# Ubuntu
sudo apt install libogre-1.12-dev
```

### Issue: Bullet Physics linking error
**Solution**: Ensure Bullet is installed with CMake support
```bash
emerge -av sci-physics/bullet
```

### Issue: Crash on startup
**Solution**: Check ogre.log for details. Usually missing resources or plugins.

## Performance Targets

### Minimum Specs (60 FPS)
- GPU: GTX 960 / R9 280
- Settings: Low-Medium

### Recommended Specs (60 FPS)
- GPU: GTX 1060 / RX 580
- Settings: High-Ultra

### Optimization Priorities
1. Reduce draw calls (batching)
2. LOD system for models
3. Frustum culling
4. Occlusion culling
5. Texture atlasing
6. Efficient particle systems

## Release Checklist

- [ ] All characters implemented
- [ ] All bosses implemented
- [ ] Story mode complete
- [ ] Multiplayer tested
- [ ] Performance optimized
- [ ] All assets finalized
- [ ] Audio implemented
- [ ] UI polished
- [ ] Bugs fixed
- [ ] Playtesting complete
- [ ] Packaging for Windows
- [ ] Packaging for Linux
- [ ] Documentation complete
- [ ] Marketing materials ready

## Questions?

See main README.md or create an issue on GitHub.
