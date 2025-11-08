#include "graphics/ParticleManager.hpp"
#include <iostream>

namespace BVA {

ParticleManager::ParticleManager(Ogre::SceneManager* sm) : sceneManager(sm) {}

ParticleManager::~ParticleManager() {
    shutdown();
}

bool ParticleManager::initialize() {
    std::cout << "Particle manager initializing..." << std::endl;

    createParticleTemplates();

    std::cout << "Particle manager initialized" << std::endl;
    return true;
}

void ParticleManager::shutdown() {
    stopAllEffects();
}

void ParticleManager::update(float dt) {
    updateActiveEffects(dt);
}

void ParticleManager::createParticleTemplates() {
    // In a full implementation, these would be defined in .particle scripts
    // For now, we'll create them programmatically

    std::cout << "Creating particle templates..." << std::endl;

    // Example: Fire particle system
    try {
        Ogre::ParticleSystem* fireTemplate = sceneManager->createParticleSystem("FireTemplate", 200);

        // Create emitter
        Ogre::ParticleEmitter* emitter = fireTemplate->addEmitter("Point");
        emitter->setParameter("emission_rate", "100");
        emitter->setParameter("angle", "30");
        emitter->setParameter("time_to_live", "2");
        emitter->setParameter("direction", "0 1 0");
        emitter->setParameter("velocity", "50");

        // Create affectors
        Ogre::ParticleAffector* scaler = fireTemplate->addAffector("Scaler");
        scaler->setParameter("rate", "50");

        Ogre::ParticleAffector* colorInterpolator = fireTemplate->addAffector("ColourInterpolator");
        colorInterpolator->setParameter("time0", "0");
        colorInterpolator->setParameter("colour0", "1 1 0");
        colorInterpolator->setParameter("time1", "0.5");
        colorInterpolator->setParameter("colour1", "1 0 0");
        colorInterpolator->setParameter("time2", "1");
        colorInterpolator->setParameter("colour2", "0 0 0");

        // Remove from scene, it's just a template
        sceneManager->destroyParticleSystem(fireTemplate);

    } catch (const Ogre::Exception& e) {
        std::cerr << "Could not create particle templates: " << e.what() << std::endl;
    }

    std::cout << "Particle templates created" << std::endl;
}

Ogre::ParticleSystem* ParticleManager::createEffect(ParticleEffect effect,
                                                     const Ogre::Vector3& position,
                                                     float duration) {
    std::string effectName;

    switch (effect) {
        case ParticleEffect::Fire:
            effectName = "FireTemplate";
            break;
        case ParticleEffect::Explosion:
            effectName = "ExplosionTemplate";
            break;
        case ParticleEffect::Impact:
            effectName = "ImpactTemplate";
            break;
        case ParticleEffect::Heal:
            effectName = "HealTemplate";
            break;
        case ParticleEffect::Buff:
            effectName = "BuffTemplate";
            break;
        case ParticleEffect::Sparkles:
            effectName = "SparklesTemplate";
            break;
        case ParticleEffect::AbilityActivation:
            effectName = "AbilityTemplate";
            break;
        default:
            effectName = "FireTemplate";
    }

    return createCustomEffect(effectName, position);
}

Ogre::ParticleSystem* ParticleManager::createCustomEffect(const std::string& templateName,
                                                           const Ogre::Vector3& position) {
    try {
        // Create unique name for this effect instance
        std::string effectId = "Effect_" + std::to_string(effectCounter++);

        // Create particle system (fallback to simple system if template doesn't exist)
        Ogre::ParticleSystem* particleSystem = nullptr;
        try {
            particleSystem = sceneManager->createParticleSystem(effectId, templateName);
        } catch (...) {
            // Create basic particle system
            particleSystem = sceneManager->createParticleSystem(effectId, 100);
            Ogre::ParticleEmitter* emitter = particleSystem->addEmitter("Point");
            emitter->setParameter("emission_rate", "50");
            emitter->setParameter("time_to_live", "1");
        }

        // Create scene node and attach
        Ogre::SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode();
        node->setPosition(position);
        node->attachObject(particleSystem);

        // Add to active effects
        ActiveEffect activeEffect;
        activeEffect.system = particleSystem;
        activeEffect.node = node;
        activeEffect.timeRemaining = 2.0f; // Default duration
        activeEffects.push_back(activeEffect);

        return particleSystem;

    } catch (const Ogre::Exception& e) {
        std::cerr << "Failed to create particle effect: " << e.what() << std::endl;
        return nullptr;
    }
}

void ParticleManager::playAbilityEffect(const std::string& characterName,
                                         const Ogre::Vector3& position) {
    // Create character-specific effect
    std::cout << "Playing ability effect for " << characterName << std::endl;

    ParticleEffect effectType = ParticleEffect::AbilityActivation;

    // Customize based on character
    if (characterName == "Bas") {
        effectType = ParticleEffect::Explosion;
    } else if (characterName == "Nitin") {
        effectType = ParticleEffect::Fire;
    } else if (characterName == "Fufinho") {
        effectType = ParticleEffect::Sparkles;
    }

    createEffect(effectType, position, 2.0f);
}

void ParticleManager::playImpactEffect(const Ogre::Vector3& position, float intensity) {
    createEffect(ParticleEffect::Impact, position, 0.5f);
}

void ParticleManager::playExplosionEffect(const Ogre::Vector3& position, float radius) {
    auto* effect = createEffect(ParticleEffect::Explosion, position, 1.5f);
    if (effect) {
        // Scale based on radius
        float scale = radius / 5.0f;
        // Note: Would need to adjust emitter parameters here
    }
}

void ParticleManager::playHealEffect(const Ogre::Vector3& position) {
    createEffect(ParticleEffect::Heal, position, 2.0f);
}

void ParticleManager::stopEffect(Ogre::ParticleSystem* effect) {
    if (!effect) return;

    auto it = std::find_if(activeEffects.begin(), activeEffects.end(),
                           [effect](const ActiveEffect& e) {
                               return e.system == effect;
                           });

    if (it != activeEffects.end()) {
        it->system->clear();
        sceneManager->destroyParticleSystem(it->system);
        sceneManager->destroySceneNode(it->node);
        activeEffects.erase(it);
    }
}

void ParticleManager::stopAllEffects() {
    for (auto& effect : activeEffects) {
        effect.system->clear();
        sceneManager->destroyParticleSystem(effect.system);
        sceneManager->destroySceneNode(effect.node);
    }
    activeEffects.clear();
}

void ParticleManager::updateActiveEffects(float dt) {
    auto it = activeEffects.begin();
    while (it != activeEffects.end()) {
        it->timeRemaining -= dt;

        if (it->timeRemaining <= 0.0f) {
            // Effect expired, clean up
            it->system->clear();
            sceneManager->destroyParticleSystem(it->system);
            sceneManager->destroySceneNode(it->node);
            it = activeEffects.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace BVA
