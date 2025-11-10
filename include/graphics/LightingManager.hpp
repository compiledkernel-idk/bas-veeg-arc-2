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
#include <vector>
#include <memory>

namespace BVA {

class LightingManager {
public:
    LightingManager(Ogre::SceneManager* sceneManager);
    ~LightingManager();

    bool initialize();
    void shutdown();
    void update(float dt);

    // Light creation
    Ogre::Light* createDirectionalLight(const Ogre::Vector3& direction,
                                         const Ogre::ColourValue& color);
    Ogre::Light* createPointLight(const Ogre::Vector3& position,
                                   const Ogre::ColourValue& color,
                                   float range = 10.0f);
    Ogre::Light* createSpotLight(const Ogre::Vector3& position,
                                  const Ogre::Vector3& direction,
                                  const Ogre::ColourValue& color,
                                  float range = 10.0f,
                                  float angle = 45.0f);

    // Dynamic lighting for effects
    Ogre::Light* createTemporaryLight(const Ogre::Vector3& position,
                                       const Ogre::ColourValue& color,
                                       float duration,
                                       float range = 5.0f);

    // Environment
    void setAmbientLight(const Ogre::ColourValue& color);
    void setSkybox(const std::string& materialName);

    // Shadows
    void setShadowTechnique(Ogre::ShadowTechnique technique);
    void setShadowQuality(int textureSize, int numCascades = 3);
    void setShadowEnabled(bool enabled);

private:
    void updateTemporaryLights(float dt);

    Ogre::SceneManager* sceneManager;

    struct TemporaryLight {
        Ogre::Light* light;
        Ogre::SceneNode* node;
        float timeRemaining;
    };

    std::vector<TemporaryLight> temporaryLights;
    std::vector<Ogre::Light*> persistentLights;

    int lightCounter = 0;
};

} // namespace BVA
