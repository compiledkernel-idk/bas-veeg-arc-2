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

#include "graphics/GraphicsEngine.hpp"
#include "graphics/PostProcessManager.hpp"
#include "graphics/ParticleManager.hpp"
#include "graphics/LightingManager.hpp"
#include "graphics/ProceduralGenerator.hpp"
#include <iostream>
#include <cmath>

namespace BVA {

GraphicsEngine::GraphicsEngine() {}

GraphicsEngine::~GraphicsEngine() {
    shutdown();
}

bool GraphicsEngine::initialize() {
    // Create Ogre root
    root = new Ogre::Root("plugins.cfg", "ogre.cfg", "ogre.log");

    // Show config dialog or load config
    if (!root->restoreConfig()) {
        if (!root->showConfigDialog()) {
            return false;
        }
    }

    // Create window
    window = root->initialise(true, "Bas Veeg Arc 3D");
    if (!window) {
        std::cerr << "Failed to create render window!" << std::endl;
        return false;
    }

    // Set up resources
    setupResources();

    // Create scene manager
    sceneManager = root->createSceneManager(Ogre::ST_GENERIC, "MainSceneManager");

    // Set up RT Shader System
    if (Ogre::RTShader::ShaderGenerator::initialize()) {
        shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shaderGenerator->addSceneManager(sceneManager);
    }

    // Set up camera
    setupCamera();

    // Set up viewport
    setupViewport();

    // Set up PBR shaders
    setupPBRShaders();

    // Create subsystems
    postProcess = std::make_unique<PostProcessManager>(viewport, sceneManager);
    if (!postProcess->initialize()) {
        std::cerr << "Failed to initialize post-process manager!" << std::endl;
        return false;
    }

    particles = std::make_unique<ParticleManager>(sceneManager);
    if (!particles->initialize()) {
        std::cerr << "Failed to initialize particle manager!" << std::endl;
        return false;
    }

    lighting = std::make_unique<LightingManager>(sceneManager);
    if (!lighting->initialize()) {
        std::cerr << "Failed to initialize lighting manager!" << std::endl;
        return false;
    }

    // Create initial scene
    createScene();

    // Initialize resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    return true;
}

void GraphicsEngine::shutdown() {
    if (lighting) {
        lighting->shutdown();
        lighting.reset();
    }

    if (particles) {
        particles->shutdown();
        particles.reset();
    }

    if (postProcess) {
        postProcess->shutdown();
        postProcess.reset();
    }

    if (shaderGenerator) {
        Ogre::RTShader::ShaderGenerator::destroy();
        shaderGenerator = nullptr;
    }

    if (root) {
        delete root;
        root = nullptr;
    }
}

void GraphicsEngine::render() {
    if (root && !window->isClosed()) {
        Ogre::WindowEventUtilities::messagePump();
        root->renderOneFrame();
    }
}

void GraphicsEngine::update(float dt) {
    if (postProcess) {
        postProcess->update(dt);
    }
    if (particles) {
        particles->update(dt);
    }
    if (lighting) {
        lighting->update(dt);
    }
}

void GraphicsEngine::setupResources() {
    Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();

    // Initialize procedural generators
    ProceduralMeshGenerator::initialize(sceneManager);
    ProceduralTextureGenerator::initialize();
    ProceduralAudioGenerator::initialize();

    // Create base materials procedurally
    ProceduralTextureGenerator::createCharacterMaterial("CharacterMaterial", Ogre::ColourValue(0.8f, 0.8f, 0.9f));
    ProceduralTextureGenerator::createGlowingMaterial("GlowingMaterial", Ogre::ColourValue(0.5f, 0.8f, 1.0f));
    ProceduralTextureGenerator::createEnergyMaterial("EnergyMaterial", Ogre::ColourValue(0.3f, 0.7f, 1.0f));
    ProceduralTextureGenerator::createMetallicMaterial("ArenaMaterial", Ogre::ColourValue(0.2f, 0.2f, 0.3f));
    ProceduralTextureGenerator::createGlowingMaterial("SkyMaterial", Ogre::ColourValue(0.3f, 0.5f, 0.8f));
}

void GraphicsEngine::setupCamera() {
    camera = sceneManager->createCamera("MainCamera");
    camera->setPosition(Ogre::Vector3(0, 10, 20));
    camera->lookAt(Ogre::Vector3(0, 0, 0));
    camera->setNearClipDistance(0.1f);
    camera->setFarClipDistance(1000.0f);
    camera->setAutoAspectRatio(true);
}

void GraphicsEngine::setupViewport() {
    viewport = window->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue(0.1f, 0.1f, 0.15f));
}

void GraphicsEngine::setupPBRShaders() {
    // Set up modern rendering pipeline
    sceneManager->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.35f));

    // Enable shadows
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
    sceneManager->setShadowTextureSize(2048);
    sceneManager->setShadowTextureCount(3);
}

void GraphicsEngine::createScene() {
    // Create procedural arena
    Ogre::ManualObject* arena = ProceduralMeshGenerator::createArena("MainArena", 50.0f);
    Ogre::SceneNode* arenaNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    arenaNode->attachObject(arena);

    // Create procedural sky dome
    Ogre::ManualObject* sky = ProceduralMeshGenerator::createSkyDome("SkyDome");
    Ogre::SceneNode* skyNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    skyNode->attachObject(sky);

    // Create main directional light (sun)
    Ogre::Light* sunLight = sceneManager->createLight("SunLight");
    sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
    sunLight->setDirection(Ogre::Vector3(-0.5f, -1.0f, -0.3f).normalisedCopy());
    sunLight->setDiffuseColour(Ogre::ColourValue(1.0f, 0.95f, 0.9f));
    sunLight->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

    // Add point lights for dramatic effect
    for (int i = 0; i < 4; i++) {
        float angle = i * M_PI / 2.0f;
        float radius = 20.0f;

        Ogre::Light* light = sceneManager->createLight("PointLight" + std::to_string(i));
        light->setType(Ogre::Light::LT_POINT);
        light->setPosition(Ogre::Vector3(cos(angle) * radius, 5.0f, sin(angle) * radius));

        // Colored lights for atmosphere
        Ogre::ColourValue colors[] = {
            Ogre::ColourValue(1.0f, 0.3f, 0.3f),  // Red
            Ogre::ColourValue(0.3f, 1.0f, 0.3f),  // Green
            Ogre::ColourValue(0.3f, 0.3f, 1.0f),  // Blue
            Ogre::ColourValue(1.0f, 1.0f, 0.3f)   // Yellow
        };

        light->setDiffuseColour(colors[i]);
        light->setSpecularColour(Ogre::ColourValue::White);
        light->setAttenuation(50.0f, 1.0f, 0.045f, 0.0075f);
    }
}

Ogre::SceneNode* GraphicsEngine::createSceneNode(const std::string& name) {
    std::string nodeName = name.empty() ? "Node_" + std::to_string(nodeCounter++) : name;
    return sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
}

Ogre::Entity* GraphicsEngine::createEntity(const std::string& meshName) {
    std::string entityName = "Entity_" + std::to_string(entityCounter++);
    return sceneManager->createEntity(entityName, meshName);
}

Ogre::MaterialPtr GraphicsEngine::createPBRMaterial(const std::string& name,
                                                     const std::string& albedoTexture,
                                                     const std::string& normalTexture,
                                                     const std::string& metallicTexture,
                                                     const std::string& roughnessTexture) {
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(name, "General");
    Ogre::Pass* pass = material->getTechnique(0)->getPass(0);

    // Set up PBR textures
    if (!albedoTexture.empty()) {
        Ogre::TextureUnitState* albedo = pass->createTextureUnitState(albedoTexture);
        albedo->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
    }

    if (!normalTexture.empty()) {
        Ogre::TextureUnitState* normal = pass->createTextureUnitState(normalTexture);
        normal->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
    }

    if (!metallicTexture.empty()) {
        Ogre::TextureUnitState* metallic = pass->createTextureUnitState(metallicTexture);
    }

    if (!roughnessTexture.empty()) {
        Ogre::TextureUnitState* roughness = pass->createTextureUnitState(roughnessTexture);
    }

    // Enable lighting
    pass->setLightingEnabled(true);
    pass->setSpecular(Ogre::ColourValue::White);
    pass->setShininess(32.0f);

    return material;
}

void GraphicsEngine::setWindowTitle(const std::string& title) {
    if (window) {
        // Note: Ogre doesn't directly support changing window title,
        // would need platform-specific code
    }
}

void GraphicsEngine::toggleFullscreen() {
    if (window) {
        // Toggle fullscreen (platform-specific)
    }
}

} // namespace BVA
