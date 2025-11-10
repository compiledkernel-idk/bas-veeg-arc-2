// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <OGRE/Ogre.h>
#include <string>
#include <vector>
#include <memory>

namespace BVA {

// Procedural mesh generation - creates all game geometry in code
class ProceduralMeshGenerator {
public:
    static void initialize(Ogre::SceneManager* sceneManager);

    // Character mesh generation
    static Ogre::ManualObject* createCharacterMesh(const std::string& name, const Ogre::ColourValue& color);
    static Ogre::ManualObject* createBossMesh(const std::string& name, float scale, const Ogre::ColourValue& color);

    // Geometric primitives with style
    static Ogre::ManualObject* createStylizedHumanoid(const std::string& name, const Ogre::ColourValue& color, float height = 2.0f);
    static Ogre::ManualObject* createWeaponEffect(const std::string& name, const Ogre::ColourValue& color);
    static Ogre::ManualObject* createProjectile(const std::string& name, const Ogre::ColourValue& color);

    // Environment
    static Ogre::ManualObject* createArena(const std::string& name, float size = 50.0f);
    static Ogre::ManualObject* createSkyDome(const std::string& name);

private:
    static Ogre::SceneManager* sm;
    static void addVertex(Ogre::ManualObject* obj, const Ogre::Vector3& pos, const Ogre::Vector3& normal, const Ogre::ColourValue& color);
    static void addQuad(Ogre::ManualObject* obj, const Ogre::Vector3& v1, const Ogre::Vector3& v2, const Ogre::Vector3& v3, const Ogre::Vector3& v4, const Ogre::ColourValue& color);
};

// Procedural texture generation - creates all textures in code
class ProceduralTextureGenerator {
public:
    static void initialize();

    // Material creation
    static Ogre::MaterialPtr createCharacterMaterial(const std::string& name, const Ogre::ColourValue& baseColor);
    static Ogre::MaterialPtr createGlowingMaterial(const std::string& name, const Ogre::ColourValue& glowColor);
    static Ogre::MaterialPtr createMetallicMaterial(const std::string& name, const Ogre::ColourValue& color);
    static Ogre::MaterialPtr createEnergyMaterial(const std::string& name, const Ogre::ColourValue& color);

    // Texture generation
    static Ogre::TexturePtr generateNoiseTexture(const std::string& name, int width = 256, int height = 256);
    static Ogre::TexturePtr generateGradientTexture(const std::string& name, const Ogre::ColourValue& color1, const Ogre::ColourValue& color2);
    static Ogre::TexturePtr generatePatternTexture(const std::string& name, const Ogre::ColourValue& color);

private:
    static float perlinNoise(float x, float y);
    static float smoothNoise(int x, int y);
};

// Procedural audio generation - creates all sounds in code
class ProceduralAudioGenerator {
public:
    static void initialize();
    static void shutdown();

    // Sound effect generation
    static std::vector<short> generateHitSound(float pitch = 1.0f);
    static std::vector<short> generateExplosionSound();
    static std::vector<short> generateLaserSound();
    static std::vector<short> generatePowerUpSound();
    static std::vector<short> generateFootstepSound();

    // Voice synthesis (simple tones for abilities)
    static std::vector<short> generateAbilitySound(float frequency, float duration);

    // Music generation (simple procedural music)
    static std::vector<short> generateBattleMusic(int beats = 64);
    static std::vector<short> generateVictoryMusic();
    static std::vector<short> generateMenuMusic();

    // Audio buffer management
    static unsigned int createAudioBuffer(const std::vector<short>& samples, int sampleRate = 44100);

private:
    static std::vector<unsigned int> audioBuffers;
    static constexpr int SAMPLE_RATE = 44100;

    static float generateWave(float frequency, float time, float phase = 0.0f);
    static float generateNoise();
    static void applyADSREnvelope(std::vector<short>& samples, float attack, float decay, float sustain, float release);
};

} // namespace BVA
