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
#include <memory>
#include <unordered_map>

namespace BVA {

enum class ParticleEffect {
    Fire,
    Smoke,
    Explosion,
    Impact,
    Heal,
    Buff,
    Debuff,
    Lightning,
    Dust,
    Blood,
    Sparkles,
    AbilityActivation
};

class ParticleManager {
public:
    ParticleManager(Ogre::SceneManager* sceneManager);
    ~ParticleManager();

    bool initialize();
    void shutdown();
    void update(float dt);

    // Particle effect creation
    Ogre::ParticleSystem* createEffect(ParticleEffect effect, const Ogre::Vector3& position,
                                        float duration = 2.0f);
    Ogre::ParticleSystem* createCustomEffect(const std::string& templateName,
                                              const Ogre::Vector3& position);

    // Character-specific ability effects
    void playAbilityEffect(const std::string& characterName, const Ogre::Vector3& position);

    // Combat effects
    void playImpactEffect(const Ogre::Vector3& position, float intensity = 1.0f);
    void playExplosionEffect(const Ogre::Vector3& position, float radius = 5.0f);
    void playHealEffect(const Ogre::Vector3& position);

    // Cleanup
    void stopEffect(Ogre::ParticleSystem* effect);
    void stopAllEffects();

private:
    void createParticleTemplates();
    void updateActiveEffects(float dt);

    Ogre::SceneManager* sceneManager;

    struct ActiveEffect {
        Ogre::ParticleSystem* system;
        Ogre::SceneNode* node;
        float timeRemaining;
    };

    std::vector<ActiveEffect> activeEffects;
    int effectCounter = 0;
};

} // namespace BVA
