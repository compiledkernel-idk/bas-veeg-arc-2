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


#include "graphics/PostProcessManager.hpp"
#include <iostream>

namespace BVA {

PostProcessManager::PostProcessManager(Ogre::Viewport* vp, Ogre::SceneManager* sm)
    : viewport(vp), sceneManager(sm) {}

PostProcessManager::~PostProcessManager() {
    shutdown();
}

bool PostProcessManager::initialize() {
    std::cout << "Post-process manager initializing..." << std::endl;

    createCompositors();

    // Set default quality
    setGraphicsQuality(GraphicsQuality::High);

    std::cout << "Post-process manager initialized" << std::endl;
    return true;
}

void PostProcessManager::shutdown() {
    // Cleanup compositors if needed
}

void PostProcessManager::update(float dt) {
    updateScreenShake(dt);
}

void PostProcessManager::createCompositors() {
    // Note: In a real implementation, you would create Ogre compositor scripts
    // For now, we'll use simplified approach

    std::cout << "Creating post-process compositor chain..." << std::endl;

    // Compositor chain would be:
    // 1. HDR rendering
    // 2. Bloom
    // 3. SSAO
    // 4. Tone mapping
    // 5. FXAA/MSAA

    // This requires compositor scripts in resources/compositors/
}

void PostProcessManager::setBloomEnabled(bool enabled) {
    bloomEnabled = enabled;
    std::cout << "Bloom: " << (enabled ? "ON" : "OFF") << std::endl;
}

void PostProcessManager::setHDREnabled(bool enabled) {
    hdrEnabled = enabled;
    std::cout << "HDR: " << (enabled ? "ON" : "OFF") << std::endl;
}

void PostProcessManager::setSSAOEnabled(bool enabled) {
    ssaoEnabled = enabled;
    std::cout << "SSAO: " << (enabled ? "ON" : "OFF") << std::endl;
}

void PostProcessManager::setMotionBlurEnabled(bool enabled) {
    motionBlurEnabled = enabled;
    std::cout << "Motion Blur: " << (enabled ? "ON" : "OFF") << std::endl;
}

void PostProcessManager::setAntiAliasingEnabled(bool enabled, int samples) {
    aaEnabled = enabled;
    std::cout << "Anti-aliasing: " << (enabled ? "ON" : "OFF")
              << " (samples: " << samples << ")" << std::endl;
}

void PostProcessManager::setGraphicsQuality(GraphicsQuality quality) {
    currentQuality = quality;

    switch (quality) {
        case GraphicsQuality::Low:
            setBloomEnabled(false);
            setHDREnabled(false);
            setSSAOEnabled(false);
            setMotionBlurEnabled(false);
            setAntiAliasingEnabled(false);
            break;

        case GraphicsQuality::Medium:
            setBloomEnabled(true);
            setHDREnabled(false);
            setSSAOEnabled(false);
            setMotionBlurEnabled(false);
            setAntiAliasingEnabled(true, 2);
            break;

        case GraphicsQuality::High:
            setBloomEnabled(true);
            setHDREnabled(true);
            setSSAOEnabled(true);
            setMotionBlurEnabled(false);
            setAntiAliasingEnabled(true, 4);
            break;

        case GraphicsQuality::Ultra:
            setBloomEnabled(true);
            setHDREnabled(true);
            setSSAOEnabled(true);
            setMotionBlurEnabled(true);
            setAntiAliasingEnabled(true, 8);
            break;
    }

    std::cout << "Graphics quality set to: " << static_cast<int>(quality) << std::endl;
}

void PostProcessManager::setBloomIntensity(float intensity) {
    bloomIntensity = std::max(0.0f, std::min(2.0f, intensity));
}

void PostProcessManager::setExposure(float exp) {
    exposure = std::max(0.1f, std::min(4.0f, exp));
}

void PostProcessManager::setGamma(float g) {
    gamma = std::max(1.0f, std::min(3.0f, g));
}

void PostProcessManager::setSaturation(float sat) {
    saturation = std::max(0.0f, std::min(2.0f, sat));
}

void PostProcessManager::setContrast(float con) {
    contrast = std::max(0.5f, std::min(2.0f, con));
}

void PostProcessManager::addScreenShake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeDuration = duration;
}

void PostProcessManager::updateScreenShake(float dt) {
    if (shakeDuration > 0.0f) {
        shakeDuration -= dt;

        // Apply screen shake to camera
        if (sceneManager && sceneManager->hasCamera("MainCamera")) {
            Ogre::Camera* cam = sceneManager->getCamera("MainCamera");

            // Random shake offset
            float offsetX = (rand() / (float)RAND_MAX - 0.5f) * shakeIntensity;
            float offsetY = (rand() / (float)RAND_MAX - 0.5f) * shakeIntensity;
            float offsetZ = (rand() / (float)RAND_MAX - 0.5f) * shakeIntensity;

            // Store original position on first shake
            static bool hasOriginal = false;
            if (!hasOriginal) {
                originalCameraPosition = cam->getPosition();
                hasOriginal = true;
            }

            // Apply shake
            cam->setPosition(originalCameraPosition + Ogre::Vector3(offsetX, offsetY, offsetZ));

            // Decrease intensity over time
            shakeIntensity *= 0.95f;
        }

        if (shakeDuration <= 0.0f) {
            shakeDuration = 0.0f;
            shakeIntensity = 0.0f;

            // Restore original camera position
            if (sceneManager && sceneManager->hasCamera("MainCamera")) {
                Ogre::Camera* cam = sceneManager->getCamera("MainCamera");
                cam->setPosition(originalCameraPosition);
            }
        }
    }
}

} // namespace BVA
