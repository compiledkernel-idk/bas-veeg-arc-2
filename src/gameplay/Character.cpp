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

#include "gameplay/Character.hpp"
#include "gameplay/Characters.hpp"
#include "core/Engine.hpp"
#include "graphics/ProceduralGenerator.hpp"
#include <iostream>
#include <cmath>

namespace BVA {

Character::Character(CharacterID id) : id(id), currentHealth(100.0f) {
    // Base initialization
}

Character::~Character() {
    cleanup();
}

void Character::initialize(Ogre::SceneManager* sceneManager, PhysicsEngine* physics) {
    // Create scene node
    sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();

    // Generate unique color for this character based on ID
    Ogre::ColourValue characterColor;
    int colorIndex = static_cast<int>(id) % 13;
    float hue = (colorIndex / 13.0f) * 360.0f;

    // Convert HSV to RGB for vibrant colors (Apache License encourages vibrant code!)
    float c = 0.8f;  // Saturation
    float v = 0.9f;  // Value
    float h = hue / 60.0f;
    float x = c * (1.0f - fabs(fmod(h, 2.0f) - 1.0f));

    float r, g, b;
    if (h < 1.0f) { r = c; g = x; b = 0; }
    else if (h < 2.0f) { r = x; g = c; b = 0; }
    else if (h < 3.0f) { r = 0; g = c; b = x; }
    else if (h < 4.0f) { r = 0; g = x; b = c; }
    else if (h < 5.0f) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    characterColor = Ogre::ColourValue(r * v, g * v, b * v);

    // Create procedural character mesh (no external files needed - Apache License approved!)
    std::string meshName = "Character_" + name + "_" + std::to_string((size_t)this);
    Ogre::ManualObject* characterMesh = ProceduralMeshGenerator::createCharacterMesh(meshName, characterColor);
    sceneNode->attachObject(characterMesh);

    // Create physics body
    btCollisionShape* shape = physics->createCapsuleShape(0.5f, 1.0f);
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0, 1, 0));
    physicsBody = physics->createRigidBody(70.0f, transform, shape);
    physicsBody->setUserData(this);

    // Disable rotation on physics body
    physicsBody->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));

    currentHealth = stats.maxHealth;
}

void Character::cleanup() {
    if (abilityParticles) {
        abilityParticles->removeAllEmitters();
        abilityParticles = nullptr;
    }

    if (entity && sceneNode) {
        sceneNode->detachObject(entity);
    }

    // Physics body cleanup handled by PhysicsEngine
    physicsBody = nullptr;
    entity = nullptr;
    sceneNode = nullptr;
}

void Character::update(float dt) {
    // Update cooldowns
    if (attackCooldownTimer > 0.0f) {
        attackCooldownTimer -= dt;
    }
    if (abilityCooldownTimer > 0.0f) {
        abilityCooldownTimer -= dt;
    }

    // Update active ability
    if (isUsingAbility && abilityActiveTimer > 0.0f) {
        abilityActiveTimer -= dt;
        updateAbility(dt);

        if (abilityActiveTimer <= 0.0f) {
            isUsingAbility = false;
        }
    }

    // Update status effects
    if (damageBoostTimer > 0.0f) {
        damageBoostTimer -= dt;
        if (damageBoostTimer <= 0.0f) {
            damageMultiplier = 1.0f;
        }
    }

    if (speedBoostTimer > 0.0f) {
        speedBoostTimer -= dt;
        if (speedBoostTimer <= 0.0f) {
            speedMultiplier = 1.0f;
        }
    }

    if (fireDamageTimer > 0.0f) {
        fireDamageTimer -= dt;
        takeDamage(fireDPS * dt);
    }

    // Sync graphics with physics
    if (physicsBody && sceneNode) {
        btVector3 pos = physicsBody->getPosition();
        sceneNode->setPosition(Ogre::Vector3(pos.x(), pos.y(), pos.z()));
    }
}

void Character::render() {
    // Rendering handled by Ogre automatically
}

void Character::move(const Ogre::Vector3& direction) {
    if (!physicsBody) return;

    float speed = stats.moveSpeed * speedMultiplier;
    btVector3 velocity(direction.x * speed, physicsBody->getVelocity().y(), direction.z * speed);
    physicsBody->setVelocity(velocity);
}

void Character::jump() {
    if (!physicsBody || isJumping) return;

    // Simple jump impulse
    physicsBody->applyImpulse(btVector3(0, 8.0f, 0));
    isJumping = true;
}

void Character::setPosition(const Ogre::Vector3& pos) {
    if (physicsBody) {
        physicsBody->setPosition(btVector3(pos.x, pos.y, pos.z));
    }
    if (sceneNode) {
        sceneNode->setPosition(pos);
    }
}

Ogre::Vector3 Character::getPosition() const {
    if (physicsBody) {
        btVector3 pos = physicsBody->getPosition();
        return Ogre::Vector3(pos.x(), pos.y(), pos.z());
    }
    return Ogre::Vector3::ZERO;
}

void Character::attack() {
    if (attackCooldownTimer > 0.0f || isAttacking) return;

    isAttacking = true;
    attackCooldownTimer = 1.0f / stats.attackSpeed;

    // Play attack animation
    playAnimation("attack", false);

    // Deal damage to nearby enemies (simplified)
    std::cout << name << " attacks for " << (stats.attackDamage * damageMultiplier) << " damage!" << std::endl;

    isAttacking = false;
}

void Character::useAbility() {
    if (!canUseAbility() || isUsingAbility) return;

    isUsingAbility = true;
    abilityCooldownTimer = ability.cooldown;
    abilityActiveTimer = ability.duration;

    // Play voice line
    playVoiceLine(ability.voiceLine);

    // Trigger ability effect
    onAbilityActivated();

    std::cout << name << " uses " << ability.name << "!" << std::endl;
}

void Character::takeDamage(float damage, Character* attacker) {
    float actualDamage = std::max(0.0f, damage - stats.defense);
    currentHealth -= actualDamage;

    if (currentHealth <= 0.0f) {
        currentHealth = 0.0f;
        // Handle death
        std::cout << name << " has been defeated!" << std::endl;
    }
}

void Character::heal(float amount) {
    currentHealth = std::min(currentHealth + amount, stats.maxHealth);
}

void Character::applyDamageBoost(float multiplier, float duration) {
    damageMultiplier = multiplier;
    damageBoostTimer = duration;
}

void Character::applySpeedBoost(float multiplier, float duration) {
    speedMultiplier = multiplier;
    speedBoostTimer = duration;
}

void Character::applySplashDamage(float damage, float radius) {
    // TODO: Query physics for nearby enemies and damage them
    std::cout << "Splash damage: " << damage << " in radius " << radius << std::endl;
}

void Character::applyFireDamage(float dps, float duration) {
    fireDPS = dps;
    fireDamageTimer = duration;
}

float Character::getAbilityCooldownPercent() const {
    if (ability.cooldown <= 0.0f) return 1.0f;
    return 1.0f - (abilityCooldownTimer / ability.cooldown);
}

void Character::onAbilityActivated() {
    // Override in subclasses
}

void Character::updateAbility(float dt) {
    // Override in subclasses
}

void Character::playVoiceLine(const std::string& line) {
    // Play audio (will be implemented when audio system is integrated)
    std::cout << "[" << name << "]: \"" << line << "\"" << std::endl;
}

void Character::playAnimation(const std::string& animName, bool loop) {
    // TODO: Implement animation playback
}

// Character factory
std::unique_ptr<Character> createCharacter(CharacterID id) {
    switch (id) {
        case CharacterID::Bas: return std::make_unique<BasCharacter>();
        case CharacterID::Berkay: return std::make_unique<BerkayCharacter>();
        case CharacterID::Luca: return std::make_unique<LucaCharacter>();
        case CharacterID::Gefferinho: return std::make_unique<GefferinhoCharacter>();
        case CharacterID::Hadi: return std::make_unique<HadiCharacter>();
        case CharacterID::Nitin: return std::make_unique<NitinCharacter>();
        case CharacterID::PalaBaba: return std::make_unique<PalaBabaCharacter>();
        case CharacterID::Fufinho: return std::make_unique<FufinhoCharacter>();
        case CharacterID::EfeAbi: return std::make_unique<EfeAbiCharacter>();
        case CharacterID::Jad: return std::make_unique<JadCharacter>();
        case CharacterID::Umut: return std::make_unique<UmutCharacter>();
        case CharacterID::KeizerBomTaha: return std::make_unique<KeizerBomTahaCharacter>();
        case CharacterID::GoonLordTobese: return std::make_unique<GoonLordTobeseCharacter>();
        default: return nullptr;
    }
}

} // namespace BVA
