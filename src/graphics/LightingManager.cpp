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


#include "graphics/LightingManager.hpp"
#include <iostream>

namespace BVA {

LightingManager::LightingManager(Ogre::SceneManager* sm) : sceneManager(sm) {}

LightingManager::~LightingManager() {
    shutdown();
}

bool LightingManager::initialize() {
    std::cout << "Lighting manager initializing..." << std::endl;

    // Set default ambient light
    setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.35f));

    // Enable shadows by default
    setShadowEnabled(true);
    setShadowQuality(2048, 3);

    std::cout << "Lighting manager initialized" << std::endl;
    return true;
}

void LightingManager::shutdown() {
    // Clean up temporary lights
    for (auto& tempLight : temporaryLights) {
        sceneManager->destroyLight(tempLight.light);
        sceneManager->destroySceneNode(tempLight.node);
    }
    temporaryLights.clear();

    // Clean up persistent lights
    for (auto* light : persistentLights) {
        sceneManager->destroyLight(light);
    }
    persistentLights.clear();
}

void LightingManager::update(float dt) {
    updateTemporaryLights(dt);
}

Ogre::Light* LightingManager::createDirectionalLight(const Ogre::Vector3& direction,
                                                      const Ogre::ColourValue& color) {
    std::string lightName = "DirectionalLight_" + std::to_string(lightCounter++);

    Ogre::Light* light = sceneManager->createLight(lightName);
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(direction.normalisedCopy());
    light->setDiffuseColour(color);
    light->setSpecularColour(color);

    persistentLights.push_back(light);

    std::cout << "Created directional light: " << lightName << std::endl;
    return light;
}

Ogre::Light* LightingManager::createPointLight(const Ogre::Vector3& position,
                                                const Ogre::ColourValue& color,
                                                float range) {
    std::string lightName = "PointLight_" + std::to_string(lightCounter++);

    Ogre::Light* light = sceneManager->createLight(lightName);
    light->setType(Ogre::Light::LT_POINT);
    light->setDiffuseColour(color);
    light->setSpecularColour(color);
    light->setAttenuation(range, 1.0f, 0.045f, 0.0075f);

    // Create node for positioning
    Ogre::SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode();
    node->setPosition(position);
    node->attachObject(light);

    persistentLights.push_back(light);

    std::cout << "Created point light: " << lightName << std::endl;
    return light;
}

Ogre::Light* LightingManager::createSpotLight(const Ogre::Vector3& position,
                                               const Ogre::Vector3& direction,
                                               const Ogre::ColourValue& color,
                                               float range,
                                               float angle) {
    std::string lightName = "SpotLight_" + std::to_string(lightCounter++);

    Ogre::Light* light = sceneManager->createLight(lightName);
    light->setType(Ogre::Light::LT_SPOTLIGHT);
    light->setDiffuseColour(color);
    light->setSpecularColour(color);
    light->setAttenuation(range, 1.0f, 0.045f, 0.0075f);
    light->setSpotlightRange(Ogre::Degree(angle), Ogre::Degree(angle * 1.3f));

    // Create node for positioning and direction
    Ogre::SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode();
    node->setPosition(position);
    node->setDirection(direction.normalisedCopy());
    node->attachObject(light);

    persistentLights.push_back(light);

    std::cout << "Created spot light: " << lightName << std::endl;
    return light;
}

Ogre::Light* LightingManager::createTemporaryLight(const Ogre::Vector3& position,
                                                     const Ogre::ColourValue& color,
                                                     float duration,
                                                     float range) {
    std::string lightName = "TempLight_" + std::to_string(lightCounter++);

    Ogre::Light* light = sceneManager->createLight(lightName);
    light->setType(Ogre::Light::LT_POINT);
    light->setDiffuseColour(color);
    light->setSpecularColour(color);
    light->setAttenuation(range, 1.0f, 0.09f, 0.032f);

    // Create node
    Ogre::SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode();
    node->setPosition(position);
    node->attachObject(light);

    // Add to temporary lights
    TemporaryLight tempLight;
    tempLight.light = light;
    tempLight.node = node;
    tempLight.timeRemaining = duration;
    temporaryLights.push_back(tempLight);

    return light;
}

void LightingManager::setAmbientLight(const Ogre::ColourValue& color) {
    sceneManager->setAmbientLight(color);
    std::cout << "Ambient light set to: (" << color.r << ", " << color.g << ", "
              << color.b << ")" << std::endl;
}

void LightingManager::setSkybox(const std::string& materialName) {
    try {
        sceneManager->setSkyBox(true, materialName, 5000);
        std::cout << "Skybox set to: " << materialName << std::endl;
    } catch (const Ogre::Exception& e) {
        std::cerr << "Failed to set skybox: " << e.what() << std::endl;
    }
}

void LightingManager::setShadowTechnique(Ogre::ShadowTechnique technique) {
    sceneManager->setShadowTechnique(technique);
    std::cout << "Shadow technique set" << std::endl;
}

void LightingManager::setShadowQuality(int textureSize, int numCascades) {
    sceneManager->setShadowTextureSize(textureSize);
    sceneManager->setShadowTextureCount(numCascades);
    sceneManager->setShadowFarDistance(100.0f);

    std::cout << "Shadow quality: " << textureSize << "x" << textureSize
              << " with " << numCascades << " cascades" << std::endl;
}

void LightingManager::setShadowEnabled(bool enabled) {
    if (enabled) {
        sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
    } else {
        sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    }
    std::cout << "Shadows: " << (enabled ? "ON" : "OFF") << std::endl;
}

void LightingManager::updateTemporaryLights(float dt) {
    auto it = temporaryLights.begin();
    while (it != temporaryLights.end()) {
        it->timeRemaining -= dt;

        if (it->timeRemaining <= 0.0f) {
            // Light expired, clean up
            sceneManager->destroyLight(it->light);
            sceneManager->destroySceneNode(it->node);
            it = temporaryLights.erase(it);
        } else {
            // Fade out light intensity
            float fadePercent = it->timeRemaining / 2.0f; // Assuming 2s max duration
            Ogre::ColourValue currentColor = it->light->getDiffuseColour();
            it->light->setDiffuseColour(currentColor * fadePercent);
            ++it;
        }
    }
}

} // namespace BVA
