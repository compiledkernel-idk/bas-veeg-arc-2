#include "graphics/GraphicsEngine.hpp"
#include "graphics/PostProcessManager.hpp"
#include "graphics/ParticleManager.hpp"
#include "graphics/LightingManager.hpp"
#include <iostream>

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

    // Add resource locations
    rgm.addResourceLocation("assets/models", "FileSystem", "General");
    rgm.addResourceLocation("assets/textures", "FileSystem", "General");
    rgm.addResourceLocation("assets/materials", "FileSystem", "General");
    rgm.addResourceLocation("shaders", "FileSystem", "General");
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
    // Create ground plane
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "ground",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane,
        100.0f, 100.0f,
        20, 20,
        true, 1,
        5.0f, 5.0f,
        Ogre::Vector3::UNIT_Z
    );

    Ogre::Entity* groundEntity = sceneManager->createEntity("ground");
    Ogre::SceneNode* groundNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    groundNode->attachObject(groundEntity);

    // Create main directional light (sun)
    Ogre::Light* sunLight = sceneManager->createLight("SunLight");
    sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
    sunLight->setDirection(Ogre::Vector3(-0.5f, -1.0f, -0.3f).normalisedCopy());
    sunLight->setDiffuseColour(Ogre::ColourValue(1.0f, 0.95f, 0.9f));
    sunLight->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
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
