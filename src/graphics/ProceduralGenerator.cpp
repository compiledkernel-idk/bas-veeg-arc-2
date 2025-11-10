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

#include "graphics/ProceduralGenerator.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <cmath>
#include <random>
#include <algorithm>

namespace BVA {

// ==================== Procedural Mesh Generator ====================

Ogre::SceneManager* ProceduralMeshGenerator::sm = nullptr;

void ProceduralMeshGenerator::initialize(Ogre::SceneManager* sceneManager) {
    sm = sceneManager;
}

void ProceduralMeshGenerator::addVertex(Ogre::ManualObject* obj, const Ogre::Vector3& pos,
                                        const Ogre::Vector3& normal, const Ogre::ColourValue& color) {
    obj->position(pos);
    obj->normal(normal);
    obj->colour(color);
    obj->textureCoord(pos.x, pos.z);
}

void ProceduralMeshGenerator::addQuad(Ogre::ManualObject* obj, const Ogre::Vector3& v1,
                                     const Ogre::Vector3& v2, const Ogre::Vector3& v3,
                                     const Ogre::Vector3& v4, const Ogre::ColourValue& color) {
    Ogre::Vector3 normal = (v2 - v1).crossProduct(v3 - v1).normalisedCopy();

    addVertex(obj, v1, normal, color);
    addVertex(obj, v2, normal, color);
    addVertex(obj, v3, normal, color);
    addVertex(obj, v4, normal, color);
}

Ogre::ManualObject* ProceduralMeshGenerator::createStylizedHumanoid(const std::string& name,
                                                                     const Ogre::ColourValue& color,
                                                                     float height) {
    auto* obj = sm->createManualObject(name);
    obj->begin("CharacterMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    float w = height * 0.3f;  // Width
    float h = height;         // Height
    float d = height * 0.2f;  // Depth

    // Body (stylized rectangular torso)
    float bodyH = h * 0.5f;
    Ogre::ColourValue bodyColor = color * 1.1f;

    // Front face
    addVertex(obj, Ogre::Vector3(-w/2, 0, d/2), Ogre::Vector3(0, 0, 1), bodyColor);
    addVertex(obj, Ogre::Vector3(w/2, 0, d/2), Ogre::Vector3(0, 0, 1), bodyColor);
    addVertex(obj, Ogre::Vector3(w/2, bodyH, d/2), Ogre::Vector3(0, 0, 1), bodyColor);
    addVertex(obj, Ogre::Vector3(-w/2, bodyH, d/2), Ogre::Vector3(0, 0, 1), bodyColor);
    obj->quad(0, 1, 2, 3);

    // Head (stylized cube)
    float headSize = h * 0.2f;
    float headY = bodyH;
    Ogre::ColourValue headColor = color * 1.2f;

    for (int i = 0; i < 6; i++) {
        float angle = i * M_PI / 3.0f;
        float nextAngle = (i + 1) * M_PI / 3.0f;
        float r = headSize;

        Ogre::Vector3 v1(r * cos(angle), headY, r * sin(angle));
        Ogre::Vector3 v2(r * cos(nextAngle), headY, r * sin(nextAngle));
        Ogre::Vector3 v3(r * cos(nextAngle), headY + headSize, r * sin(nextAngle));
        Ogre::Vector3 v4(r * cos(angle), headY + headSize, r * sin(angle));

        addQuad(obj, v1, v2, v3, v4, headColor);
    }

    // Arms (stylized cylinders)
    float armW = w * 0.15f;
    float armL = bodyH * 0.8f;
    Ogre::ColourValue armColor = color * 0.9f;

    // Left arm
    for (int i = 0; i < 6; i++) {
        float angle = i * M_PI / 3.0f;
        float nextAngle = (i + 1) * M_PI / 3.0f;

        Ogre::Vector3 v1(-w/2 - armW * cos(angle), bodyH * 0.7f, armW * sin(angle));
        Ogre::Vector3 v2(-w/2 - armW * cos(nextAngle), bodyH * 0.7f, armW * sin(nextAngle));
        Ogre::Vector3 v3(-w/2 - armW * cos(nextAngle), bodyH * 0.7f - armL, armW * sin(nextAngle));
        Ogre::Vector3 v4(-w/2 - armW * cos(angle), bodyH * 0.7f - armL, armW * sin(angle));

        addQuad(obj, v1, v2, v3, v4, armColor);
    }

    // Right arm (mirrored)
    for (int i = 0; i < 6; i++) {
        float angle = i * M_PI / 3.0f;
        float nextAngle = (i + 1) * M_PI / 3.0f;

        Ogre::Vector3 v1(w/2 + armW * cos(angle), bodyH * 0.7f, armW * sin(angle));
        Ogre::Vector3 v2(w/2 + armW * cos(nextAngle), bodyH * 0.7f, armW * sin(nextAngle));
        Ogre::Vector3 v3(w/2 + armW * cos(nextAngle), bodyH * 0.7f - armL, armW * sin(nextAngle));
        Ogre::Vector3 v4(w/2 + armW * cos(angle), bodyH * 0.7f - armL, armW * sin(angle));

        addQuad(obj, v1, v2, v3, v4, armColor);
    }

    // Legs
    float legW = w * 0.2f;
    float legL = h * 0.5f;

    // Left leg
    for (int i = 0; i < 6; i++) {
        float angle = i * M_PI / 3.0f;
        float nextAngle = (i + 1) * M_PI / 3.0f;

        Ogre::Vector3 v1(-w/4 + legW * cos(angle), 0, legW * sin(angle));
        Ogre::Vector3 v2(-w/4 + legW * cos(nextAngle), 0, legW * sin(nextAngle));
        Ogre::Vector3 v3(-w/4 + legW * cos(nextAngle), -legL, legW * sin(nextAngle));
        Ogre::Vector3 v4(-w/4 + legW * cos(angle), -legL, legW * sin(angle));

        addQuad(obj, v1, v2, v3, v4, armColor);
    }

    // Right leg
    for (int i = 0; i < 6; i++) {
        float angle = i * M_PI / 3.0f;
        float nextAngle = (i + 1) * M_PI / 3.0f;

        Ogre::Vector3 v1(w/4 + legW * cos(angle), 0, legW * sin(angle));
        Ogre::Vector3 v2(w/4 + legW * cos(nextAngle), 0, legW * sin(nextAngle));
        Ogre::Vector3 v3(w/4 + legW * cos(nextAngle), -legL, legW * sin(nextAngle));
        Ogre::Vector3 v4(w/4 + legW * cos(angle), -legL, legW * sin(angle));

        addQuad(obj, v1, v2, v3, v4, armColor);
    }

    obj->end();
    return obj;
}

Ogre::ManualObject* ProceduralMeshGenerator::createCharacterMesh(const std::string& name,
                                                                  const Ogre::ColourValue& color) {
    return createStylizedHumanoid(name, color, 2.0f);
}

Ogre::ManualObject* ProceduralMeshGenerator::createBossMesh(const std::string& name,
                                                             float scale,
                                                             const Ogre::ColourValue& color) {
    return createStylizedHumanoid(name, color, 3.0f * scale);
}

Ogre::ManualObject* ProceduralMeshGenerator::createWeaponEffect(const std::string& name,
                                                                 const Ogre::ColourValue& color) {
    auto* obj = sm->createManualObject(name);
    obj->begin("EnergyMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    // Create energy blade/trail effect
    int segments = 12;
    float length = 2.0f;
    float width = 0.3f;

    for (int i = 0; i < segments; i++) {
        float t = (float)i / segments;
        float nextT = (float)(i + 1) / segments;

        float y1 = t * length;
        float y2 = nextT * length;
        float w1 = width * (1.0f - t);
        float w2 = width * (1.0f - nextT);

        Ogre::ColourValue c = color;
        c.a = 1.0f - t * 0.8f;

        addVertex(obj, Ogre::Vector3(-w1, y1, 0), Ogre::Vector3(0, 0, 1), c);
        addVertex(obj, Ogre::Vector3(w1, y1, 0), Ogre::Vector3(0, 0, 1), c);

        c.a = 1.0f - nextT * 0.8f;
        addVertex(obj, Ogre::Vector3(w2, y2, 0), Ogre::Vector3(0, 0, 1), c);
        addVertex(obj, Ogre::Vector3(-w2, y2, 0), Ogre::Vector3(0, 0, 1), c);

        int base = i * 4;
        obj->quad(base, base + 1, base + 2, base + 3);
    }

    obj->end();
    return obj;
}

Ogre::ManualObject* ProceduralMeshGenerator::createProjectile(const std::string& name,
                                                               const Ogre::ColourValue& color) {
    auto* obj = sm->createManualObject(name);
    obj->begin("GlowingMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    // Create glowing sphere-like projectile
    int segments = 16;
    int rings = 8;
    float radius = 0.3f;

    for (int r = 0; r < rings; r++) {
        for (int s = 0; s < segments; s++) {
            float theta1 = r * M_PI / rings;
            float theta2 = (r + 1) * M_PI / rings;
            float phi1 = s * 2 * M_PI / segments;
            float phi2 = (s + 1) * 2 * M_PI / segments;

            Ogre::Vector3 v1(radius * sin(theta1) * cos(phi1), radius * cos(theta1), radius * sin(theta1) * sin(phi1));
            Ogre::Vector3 v2(radius * sin(theta1) * cos(phi2), radius * cos(theta1), radius * sin(theta1) * sin(phi2));
            Ogre::Vector3 v3(radius * sin(theta2) * cos(phi2), radius * cos(theta2), radius * sin(theta2) * sin(phi2));
            Ogre::Vector3 v4(radius * sin(theta2) * cos(phi1), radius * cos(theta2), radius * sin(theta2) * sin(phi1));

            addQuad(obj, v1, v2, v3, v4, color);
        }
    }

    obj->end();
    return obj;
}

Ogre::ManualObject* ProceduralMeshGenerator::createArena(const std::string& name, float size) {
    auto* obj = sm->createManualObject(name);
    obj->begin("ArenaMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    // Create stylized arena floor with grid pattern
    int divisions = 20;
    float cellSize = size / divisions;

    for (int x = 0; x < divisions; x++) {
        for (int z = 0; z < divisions; z++) {
            float x1 = (x - divisions/2) * cellSize;
            float x2 = (x + 1 - divisions/2) * cellSize;
            float z1 = (z - divisions/2) * cellSize;
            float z2 = (z + 1 - divisions/2) * cellSize;

            // Checkerboard pattern
            Ogre::ColourValue color = ((x + z) % 2 == 0) ?
                Ogre::ColourValue(0.2f, 0.2f, 0.3f) :
                Ogre::ColourValue(0.15f, 0.15f, 0.25f);

            // Add glow to grid lines
            if (x % 2 == 0 || z % 2 == 0) {
                color += Ogre::ColourValue(0.05f, 0.1f, 0.2f);
            }

            Ogre::Vector3 v1(x1, 0, z1);
            Ogre::Vector3 v2(x2, 0, z1);
            Ogre::Vector3 v3(x2, 0, z2);
            Ogre::Vector3 v4(x1, 0, z2);

            addQuad(obj, v1, v2, v3, v4, color);
        }
    }

    // Arena walls with energy effect
    float wallHeight = 5.0f;
    Ogre::ColourValue wallColor(0.1f, 0.3f, 0.5f, 0.3f);

    // North wall
    addQuad(obj,
        Ogre::Vector3(-size/2, 0, size/2),
        Ogre::Vector3(size/2, 0, size/2),
        Ogre::Vector3(size/2, wallHeight, size/2),
        Ogre::Vector3(-size/2, wallHeight, size/2),
        wallColor);

    obj->end();
    return obj;
}

Ogre::ManualObject* ProceduralMeshGenerator::createSkyDome(const std::string& name) {
    auto* obj = sm->createManualObject(name);
    obj->begin("SkyMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    // Create gradient sky dome
    int segments = 32;
    int rings = 16;
    float radius = 500.0f;

    for (int r = 0; r < rings / 2; r++) {  // Only top half for dome
        for (int s = 0; s < segments; s++) {
            float theta1 = r * M_PI / rings;
            float theta2 = (r + 1) * M_PI / rings;
            float phi1 = s * 2 * M_PI / segments;
            float phi2 = (s + 1) * 2 * M_PI / segments;

            // Gradient from dark blue to cyan
            float t1 = (float)r / (rings / 2);
            float t2 = (float)(r + 1) / (rings / 2);
            Ogre::ColourValue color1 = Ogre::ColourValue(0.1f, 0.2f, 0.4f) * (1.0f - t1) + Ogre::ColourValue(0.3f, 0.5f, 0.8f) * t1;
            Ogre::ColourValue color2 = Ogre::ColourValue(0.1f, 0.2f, 0.4f) * (1.0f - t2) + Ogre::ColourValue(0.3f, 0.5f, 0.8f) * t2;

            Ogre::Vector3 v1(radius * sin(theta1) * cos(phi1), radius * cos(theta1), radius * sin(theta1) * sin(phi1));
            Ogre::Vector3 v2(radius * sin(theta1) * cos(phi2), radius * cos(theta1), radius * sin(theta1) * sin(phi2));
            Ogre::Vector3 v3(radius * sin(theta2) * cos(phi2), radius * cos(theta2), radius * sin(theta2) * sin(phi2));
            Ogre::Vector3 v4(radius * sin(theta2) * cos(phi1), radius * cos(theta2), radius * sin(theta2) * sin(phi1));

            Ogre::Vector3 normal = Ogre::Vector3::ZERO - v1;
            normal.normalise();

            addVertex(obj, v1, normal, color1);
            addVertex(obj, v2, normal, color1);
            addVertex(obj, v3, normal, color2);
            addVertex(obj, v4, normal, color2);

            int base = (r * segments + s) * 4;
            obj->quad(base, base + 1, base + 2, base + 3);
        }
    }

    obj->end();
    return obj;
}

// ==================== Procedural Texture Generator ====================

void ProceduralTextureGenerator::initialize() {
    // Initialize materials
}

float ProceduralTextureGenerator::perlinNoise(float x, float y) {
    int xi = (int)x;
    int yi = (int)y;
    float xf = x - xi;
    float yf = y - yi;

    float u = xf * xf * (3.0f - 2.0f * xf);
    float v = yf * yf * (3.0f - 2.0f * yf);

    float n00 = smoothNoise(xi, yi);
    float n10 = smoothNoise(xi + 1, yi);
    float n01 = smoothNoise(xi, yi + 1);
    float n11 = smoothNoise(xi + 1, yi + 1);

    float nx0 = n00 * (1 - u) + n10 * u;
    float nx1 = n01 * (1 - u) + n11 * u;

    return nx0 * (1 - v) + nx1 * v;
}

float ProceduralTextureGenerator::smoothNoise(int x, int y) {
    static std::mt19937 rng(12345);
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int n = x + y * 57;
    n = (n << 13) ^ n;
    return 1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
}

Ogre::MaterialPtr ProceduralTextureGenerator::createCharacterMaterial(const std::string& name,
                                                                       const Ogre::ColourValue& baseColor) {
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
    pass->setDiffuse(baseColor);
    pass->setAmbient(baseColor * 0.5f);
    pass->setSpecular(Ogre::ColourValue::White);
    pass->setShininess(32.0f);
    pass->setLightingEnabled(true);

    return mat;
}

Ogre::MaterialPtr ProceduralTextureGenerator::createGlowingMaterial(const std::string& name,
                                                                     const Ogre::ColourValue& glowColor) {
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
    pass->setDiffuse(glowColor);
    pass->setAmbient(glowColor);
    pass->setEmissive(glowColor * 2.0f);
    pass->setSpecular(Ogre::ColourValue::White);
    pass->setShininess(64.0f);
    pass->setSceneBlending(Ogre::SBT_ADD);
    pass->setDepthWriteEnabled(false);

    return mat;
}

Ogre::MaterialPtr ProceduralTextureGenerator::createMetallicMaterial(const std::string& name,
                                                                      const Ogre::ColourValue& color) {
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
    pass->setDiffuse(color * 0.7f);
    pass->setAmbient(color * 0.3f);
    pass->setSpecular(Ogre::ColourValue::White);
    pass->setShininess(128.0f);
    pass->setLightingEnabled(true);

    return mat;
}

Ogre::MaterialPtr ProceduralTextureGenerator::createEnergyMaterial(const std::string& name,
                                                                    const Ogre::ColourValue& color) {
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
    pass->setDiffuse(color);
    pass->setEmissive(color * 3.0f);
    pass->setSceneBlending(Ogre::SBT_ADD);
    pass->setDepthWriteEnabled(false);
    pass->setLightingEnabled(false);

    return mat;
}

// ==================== Procedural Audio Generator ====================

std::vector<unsigned int> ProceduralAudioGenerator::audioBuffers;

void ProceduralAudioGenerator::initialize() {
    // Audio buffers will be created on-demand
}

void ProceduralAudioGenerator::shutdown() {
    for (auto buffer : audioBuffers) {
        alDeleteBuffers(1, &buffer);
    }
    audioBuffers.clear();
}

float ProceduralAudioGenerator::generateWave(float frequency, float time, float phase) {
    return sin(2.0f * M_PI * frequency * time + phase);
}

float ProceduralAudioGenerator::generateNoise() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(rng);
}

void ProceduralAudioGenerator::applyADSREnvelope(std::vector<short>& samples,
                                                  float attack, float decay,
                                                  float sustain, float release) {
    int attackSamples = (int)(attack * SAMPLE_RATE);
    int decaySamples = (int)(decay * SAMPLE_RATE);
    int releaseSamples = (int)(release * SAMPLE_RATE);
    int totalSamples = samples.size();
    int sustainSamples = totalSamples - attackSamples - decaySamples - releaseSamples;

    for (int i = 0; i < totalSamples; i++) {
        float envelope = 1.0f;

        if (i < attackSamples) {
            envelope = (float)i / attackSamples;
        } else if (i < attackSamples + decaySamples) {
            float t = (float)(i - attackSamples) / decaySamples;
            envelope = 1.0f - t * (1.0f - sustain);
        } else if (i < attackSamples + decaySamples + sustainSamples) {
            envelope = sustain;
        } else {
            float t = (float)(i - attackSamples - decaySamples - sustainSamples) / releaseSamples;
            envelope = sustain * (1.0f - t);
        }

        samples[i] = (short)(samples[i] * envelope);
    }
}

std::vector<short> ProceduralAudioGenerator::generateHitSound(float pitch) {
    float duration = 0.1f;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        float frequency = 200.0f * pitch * (1.0f - t * 2.0f);  // Falling pitch
        float wave = generateWave(frequency, t) * 0.5f + generateNoise() * 0.5f;
        samples[i] = (short)(wave * 32767.0f * (1.0f - t / duration));
    }

    return samples;
}

std::vector<short> ProceduralAudioGenerator::generateExplosionSound() {
    float duration = 0.8f;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        float decay = 1.0f - t / duration;

        // Low rumble + noise
        float rumble = generateWave(60.0f, t) * 0.3f;
        float noise = generateNoise() * 0.7f;

        samples[i] = (short)((rumble + noise) * 32767.0f * decay);
    }

    return samples;
}

std::vector<short> ProceduralAudioGenerator::generateLaserSound() {
    float duration = 0.3f;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        float frequency = 800.0f * (1.0f - t * 0.8f);  // Descending frequency
        float wave = generateWave(frequency, t);
        samples[i] = (short)(wave * 32767.0f * (1.0f - t / duration));
    }

    return samples;
}

std::vector<short> ProceduralAudioGenerator::generatePowerUpSound() {
    float duration = 0.5f;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        float frequency = 400.0f * (1.0f + t * 2.0f);  // Rising frequency
        float wave = generateWave(frequency, t) + generateWave(frequency * 2.0f, t) * 0.3f;
        samples[i] = (short)(wave * 32767.0f * 0.5f);
    }

    applyADSREnvelope(samples, 0.05f, 0.1f, 0.7f, 0.2f);
    return samples;
}

std::vector<short> ProceduralAudioGenerator::generateFootstepSound() {
    float duration = 0.15f;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        float noise = generateNoise() * 0.8f;
        float thump = generateWave(80.0f, t) * 0.2f;
        samples[i] = (short)((noise + thump) * 32767.0f * (1.0f - t / duration));
    }

    return samples;
}

std::vector<short> ProceduralAudioGenerator::generateAbilitySound(float frequency, float duration) {
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        // Complex wave with harmonics for interesting sound
        float wave = generateWave(frequency, t) * 0.6f;
        wave += generateWave(frequency * 1.5f, t) * 0.3f;
        wave += generateWave(frequency * 2.0f, t) * 0.1f;
        samples[i] = (short)(wave * 32767.0f);
    }

    applyADSREnvelope(samples, 0.1f, 0.2f, 0.6f, 0.3f);
    return samples;
}

std::vector<short> ProceduralAudioGenerator::generateBattleMusic(int beats) {
    float bpm = 140.0f;
    float beatDuration = 60.0f / bpm;
    float duration = beats * beatDuration;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    // Simple 4-chord progression
    float chords[] = {220.0f, 246.94f, 261.63f, 293.66f};  // A, B, C, D

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        int beat = (int)(t / beatDuration) % 4;
        float baseFreq = chords[beat];

        // Bass
        float bass = generateWave(baseFreq, t) * 0.3f;

        // Melody
        float melody = generateWave(baseFreq * 2.0f, t) * 0.2f;
        melody += generateWave(baseFreq * 3.0f, t) * 0.1f;

        // Percussion (noise on beats)
        float beatPhase = fmod(t, beatDuration);
        float percussion = 0.0f;
        if (beatPhase < 0.05f) {
            percussion = generateNoise() * 0.4f * (1.0f - beatPhase / 0.05f);
        }

        samples[i] = (short)((bass + melody + percussion) * 32767.0f * 0.5f);
    }

    return samples;
}

std::vector<short> ProceduralAudioGenerator::generateVictoryMusic() {
    float duration = 3.0f;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    // Triumphant ascending melody
    float notes[] = {261.63f, 293.66f, 329.63f, 349.23f, 392.00f, 440.00f, 493.88f, 523.25f};  // C major scale

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        int noteIndex = (int)(t / 0.3f) % 8;
        float frequency = notes[noteIndex];

        float wave = generateWave(frequency, t) * 0.6f;
        wave += generateWave(frequency * 2.0f, t) * 0.2f;

        samples[i] = (short)(wave * 32767.0f * 0.7f);
    }

    return samples;
}

std::vector<short> ProceduralAudioGenerator::generateMenuMusic() {
    float duration = 10.0f;
    int numSamples = (int)(duration * SAMPLE_RATE);
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;

        // Ambient pad
        float pad = generateWave(110.0f, t) * 0.1f;
        pad += generateWave(165.0f, t) * 0.1f;
        pad += generateWave(220.0f, t) * 0.05f;

        samples[i] = (short)(pad * 32767.0f);
    }

    return samples;
}

unsigned int ProceduralAudioGenerator::createAudioBuffer(const std::vector<short>& samples, int sampleRate) {
    unsigned int buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, samples.data(), samples.size() * sizeof(short), sampleRate);
    audioBuffers.push_back(buffer);
    return buffer;
}

} // namespace BVA
