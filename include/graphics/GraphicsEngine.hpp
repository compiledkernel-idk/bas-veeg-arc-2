#pragma once

#include <OGRE/Ogre.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <OGRE/Bites/OgreApplicationContext.h>
#include <memory>
#include <string>

namespace BVA {

class PostProcessManager;
class ParticleManager;
class LightingManager;

class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();

    bool initialize();
    void shutdown();
    void render();
    void update(float dt);

    // Scene management
    Ogre::SceneManager* getSceneManager() { return sceneManager; }
    Ogre::Camera* getCamera() { return camera; }
    Ogre::SceneNode* getRootSceneNode() { return sceneManager->getRootSceneNode(); }

    // Entity creation
    Ogre::SceneNode* createSceneNode(const std::string& name = "");
    Ogre::Entity* createEntity(const std::string& meshName);

    // Material system (PBR)
    Ogre::MaterialPtr createPBRMaterial(const std::string& name,
                                         const std::string& albedoTexture,
                                         const std::string& normalTexture,
                                         const std::string& metallicTexture,
                                         const std::string& roughnessTexture);

    // Subsystem accessors
    PostProcessManager* getPostProcess() { return postProcess.get(); }
    ParticleManager* getParticles() { return particles.get(); }
    LightingManager* getLighting() { return lighting.get(); }

    // Window management
    Ogre::RenderWindow* getWindow() { return window; }
    void setWindowTitle(const std::string& title);
    void toggleFullscreen();

private:
    void setupResources();
    void createScene();
    void setupCamera();
    void setupViewport();
    void setupPBRShaders();

    Ogre::Root* root = nullptr;
    Ogre::RenderWindow* window = nullptr;
    Ogre::SceneManager* sceneManager = nullptr;
    Ogre::Camera* camera = nullptr;
    Ogre::Viewport* viewport = nullptr;
    Ogre::RTShader::ShaderGenerator* shaderGenerator = nullptr;

    std::unique_ptr<PostProcessManager> postProcess;
    std::unique_ptr<ParticleManager> particles;
    std::unique_ptr<LightingManager> lighting;

    int entityCounter = 0;
    int nodeCounter = 0;
};

} // namespace BVA
