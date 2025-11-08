#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace BVA {

class AudioSource;
class AudioBuffer;

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    bool initialize();
    void shutdown();
    void update(float dt);

    // Audio loading
    AudioBuffer* loadSound(const std::string& filename);
    AudioBuffer* loadMusic(const std::string& filename);

    // 3D audio
    void setListenerPosition(float x, float y, float z);
    void setListenerVelocity(float x, float y, float z);
    void setListenerOrientation(float forwardX, float forwardY, float forwardZ,
                                 float upX, float upY, float upZ);

    // Playback
    AudioSource* playSound(const std::string& soundName, bool loop = false);
    AudioSource* playSound3D(const std::string& soundName, float x, float y, float z,
                              bool loop = false);
    AudioSource* playMusic(const std::string& musicName, bool loop = true);

    void stopAllSounds();
    void setMasterVolume(float volume);
    void setSFXVolume(float volume);
    void setMusicVolume(float volume);

private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;

    std::unordered_map<std::string, std::unique_ptr<AudioBuffer>> buffers;
    std::vector<std::unique_ptr<AudioSource>> sources;

    float masterVolume = 1.0f;
    float sfxVolume = 1.0f;
    float musicVolume = 0.7f;
};

class AudioBuffer {
public:
    AudioBuffer(const std::string& filename);
    ~AudioBuffer();

    ALuint getBufferID() const { return buffer; }
    bool isLoaded() const { return loaded; }

private:
    bool loadWAV(const std::string& filename);
    bool loadOGG(const std::string& filename);

    ALuint buffer = 0;
    bool loaded = false;
};

class AudioSource {
public:
    AudioSource();
    ~AudioSource();

    void setBuffer(AudioBuffer* buffer);
    void play();
    void pause();
    void stop();
    void setLooping(bool loop);
    void setVolume(float volume);
    void setPitch(float pitch);
    void setPosition(float x, float y, float z);
    void setVelocity(float x, float y, float z);

    bool isPlaying() const;
    bool isPaused() const;

private:
    ALuint source = 0;
};

} // namespace BVA
