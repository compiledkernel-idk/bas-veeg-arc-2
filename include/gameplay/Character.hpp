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

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <OGRE/Ogre.h>
#include "physics/PhysicsEngine.hpp"

namespace BVA {

enum class CharacterID {
    Bas,
    Berkay,
    Luca,
    Gefferinho,
    Hadi,
    Nitin,
    PalaBaba,
    Fufinho,
    EfeAbi,
    Jad,
    Umut,
    KeizerBomTaha,
    GoonLordTobese
};

enum class AbilityEffect {
    DamageBoost,
    SpeedBoost,
    HealthBoost,
    SplashDamage,
    FireDamage,
    Projectile,
    PlaneSummon
};

struct AbilityData {
    std::string name;
    std::string voiceLine;
    std::vector<std::pair<AbilityEffect, float>> effects;
    float duration;
    float cooldown;
    std::string description;
};

struct CharacterStats {
    float maxHealth = 100.0f;
    float moveSpeed = 5.0f;
    float attackDamage = 10.0f;
    float attackSpeed = 1.0f;
    float defense = 0.0f;
};

class Character {
public:
    Character(CharacterID id);
    virtual ~Character();

    // Initialization
    virtual void initialize(Ogre::SceneManager* sceneManager, PhysicsEngine* physics);
    virtual void cleanup();

    // Update
    virtual void update(float dt);
    virtual void render();

    // Movement
    void move(const Ogre::Vector3& direction);
    void jump();
    void setPosition(const Ogre::Vector3& pos);
    Ogre::Vector3 getPosition() const;

    // Combat
    void attack();
    void useAbility();
    void takeDamage(float damage, Character* attacker = nullptr);
    void heal(float amount);
    bool isAlive() const { return currentHealth > 0.0f; }

    // Status effects
    void applyDamageBoost(float multiplier, float duration);
    void applySpeedBoost(float multiplier, float duration);
    void applySplashDamage(float damage, float radius);
    void applyFireDamage(float dps, float duration);

    // Getters
    CharacterID getID() const { return id; }
    const std::string& getName() const { return name; }
    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return stats.maxHealth; }
    float getHealthPercent() const { return currentHealth / stats.maxHealth; }
    bool canUseAbility() const { return abilityCooldownTimer <= 0.0f; }
    float getAbilityCooldownPercent() const;
    const CharacterStats& getStats() const { return stats; }
    Ogre::SceneNode* getSceneNode() { return sceneNode; }

protected:
    virtual void onAbilityActivated();
    virtual void updateAbility(float dt);
    void playVoiceLine(const std::string& line);
    void playAnimation(const std::string& animName, bool loop = false);

    CharacterID id;
    std::string name;
    CharacterStats stats;
    AbilityData ability;

    // Current state
    float currentHealth;
    bool isJumping = false;
    bool isAttacking = false;
    bool isUsingAbility = false;

    // Cooldowns
    float attackCooldownTimer = 0.0f;
    float abilityCooldownTimer = 0.0f;
    float abilityActiveTimer = 0.0f;

    // Active effects
    float damageMultiplier = 1.0f;
    float speedMultiplier = 1.0f;
    float damageBoostTimer = 0.0f;
    float speedBoostTimer = 0.0f;
    float fireDamageTimer = 0.0f;
    float fireDPS = 0.0f;

    // Graphics and physics
    Ogre::SceneNode* sceneNode = nullptr;
    Ogre::Entity* entity = nullptr;
    Ogre::AnimationState* currentAnimation = nullptr;
    PhysicsBody* physicsBody = nullptr;

    // Particle effects
    Ogre::ParticleSystem* abilityParticles = nullptr;
};

// Character factory
std::unique_ptr<Character> createCharacter(CharacterID id);

} // namespace BVA
