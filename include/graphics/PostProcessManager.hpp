#pragma once

#include <OGRE/Ogre.h>
#include <memory>

namespace BVA {

enum class GraphicsQuality {
    Low,
    Medium,
    High,
    Ultra
};

class PostProcessManager {
public:
    PostProcessManager(Ogre::Viewport* viewport, Ogre::SceneManager* sceneManager);
    ~PostProcessManager();

    bool initialize();
    void shutdown();
    void update(float dt);

    // Post-processing effects
    void setBloomEnabled(bool enabled);
    void setHDREnabled(bool enabled);
    void setSSAOEnabled(bool enabled);
    void setMotionBlurEnabled(bool enabled);
    void setAntiAliasingEnabled(bool enabled, int samples = 4);

    // Graphics quality presets
    void setGraphicsQuality(GraphicsQuality quality);
    GraphicsQuality getGraphicsQuality() const { return currentQuality; }

    // Individual settings
    void setBloomIntensity(float intensity);
    void setExposure(float exposure);
    void setGamma(float gamma);
    void setSaturation(float saturation);
    void setContrast(float contrast);

    // Screen shake (for combat feedback)
    void addScreenShake(float intensity, float duration);

private:
    void createCompositors();
    void updateScreenShake(float dt);

    Ogre::Viewport* viewport;
    Ogre::SceneManager* sceneManager;

    bool bloomEnabled = true;
    bool hdrEnabled = true;
    bool ssaoEnabled = true;
    bool motionBlurEnabled = false;
    bool aaEnabled = true;

    float bloomIntensity = 1.0f;
    float exposure = 1.0f;
    float gamma = 2.2f;
    float saturation = 1.0f;
    float contrast = 1.0f;

    GraphicsQuality currentQuality = GraphicsQuality::High;

    // Screen shake
    float shakeIntensity = 0.0f;
    float shakeDuration = 0.0f;
    Ogre::Vector3 originalCameraPosition;
};

} // namespace BVA
