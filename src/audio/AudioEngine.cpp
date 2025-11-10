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


#include "audio/AudioEngine.hpp"
#include <iostream>
#include <fstream>

namespace BVA {

AudioEngine::AudioEngine() {}

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::initialize() {
    // Open default audio device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device!" << std::endl;
        return false;
    }

    // Create context
    context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create audio context!" << std::endl;
        alcCloseDevice(device);
        return false;
    }

    alcMakeContextCurrent(context);

    // Set default listener orientation
    setListenerPosition(0.0f, 0.0f, 0.0f);
    setListenerOrientation(0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    std::cout << "Audio engine initialized with OpenAL" << std::endl;
    std::cout << "  Device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;

    return true;
}

void AudioEngine::shutdown() {
    stopAllSounds();

    sources.clear();
    buffers.clear();

    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }

    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
}

void AudioEngine::update(float dt) {
    // Clean up finished sources
    auto it = sources.begin();
    while (it != sources.end()) {
        if (!(*it)->isPlaying() && !(*it)->isPaused()) {
            it = sources.erase(it);
        } else {
            ++it;
        }
    }
}

AudioBuffer* AudioEngine::loadSound(const std::string& filename) {
    // Check if already loaded
    auto it = buffers.find(filename);
    if (it != buffers.end()) {
        return it->second.get();
    }

    // Load new buffer
    auto buffer = std::make_unique<AudioBuffer>(filename);
    if (!buffer->isLoaded()) {
        std::cerr << "Failed to load audio file: " << filename << std::endl;
        return nullptr;
    }

    AudioBuffer* bufferPtr = buffer.get();
    buffers[filename] = std::move(buffer);
    return bufferPtr;
}

AudioBuffer* AudioEngine::loadMusic(const std::string& filename) {
    // Same as loadSound for now, could implement streaming later
    return loadSound(filename);
}

void AudioEngine::setListenerPosition(float x, float y, float z) {
    alListener3f(AL_POSITION, x, y, z);
}

void AudioEngine::setListenerVelocity(float x, float y, float z) {
    alListener3f(AL_VELOCITY, x, y, z);
}

void AudioEngine::setListenerOrientation(float forwardX, float forwardY, float forwardZ,
                                           float upX, float upY, float upZ) {
    ALfloat orientation[] = {forwardX, forwardY, forwardZ, upX, upY, upZ};
    alListenerfv(AL_ORIENTATION, orientation);
}

AudioSource* AudioEngine::playSound(const std::string& soundName, bool loop) {
    AudioBuffer* buffer = loadSound(soundName);
    if (!buffer) return nullptr;

    auto source = std::make_unique<AudioSource>();
    source->setBuffer(buffer);
    source->setLooping(loop);
    source->setVolume(sfxVolume * masterVolume);
    source->play();

    AudioSource* sourcePtr = source.get();
    sources.push_back(std::move(source));
    return sourcePtr;
}

AudioSource* AudioEngine::playSound3D(const std::string& soundName, float x, float y, float z,
                                       bool loop) {
    AudioSource* source = playSound(soundName, loop);
    if (source) {
        source->setPosition(x, y, z);
    }
    return source;
}

AudioSource* AudioEngine::playMusic(const std::string& musicName, bool loop) {
    AudioBuffer* buffer = loadMusic(musicName);
    if (!buffer) return nullptr;

    auto source = std::make_unique<AudioSource>();
    source->setBuffer(buffer);
    source->setLooping(loop);
    source->setVolume(musicVolume * masterVolume);
    source->play();

    AudioSource* sourcePtr = source.get();
    sources.push_back(std::move(source));
    return sourcePtr;
}

void AudioEngine::stopAllSounds() {
    sources.clear();
}

void AudioEngine::setMasterVolume(float volume) {
    masterVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioEngine::setSFXVolume(float volume) {
    sfxVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioEngine::setMusicVolume(float volume) {
    musicVolume = std::max(0.0f, std::min(1.0f, volume));
}

// AudioBuffer implementation
AudioBuffer::AudioBuffer(const std::string& filename) {
    alGenBuffers(1, &buffer);

    // Determine file type and load
    if (filename.ends_with(".wav")) {
        loaded = loadWAV(filename);
    } else if (filename.ends_with(".ogg")) {
        loaded = loadOGG(filename);
    }
}

AudioBuffer::~AudioBuffer() {
    if (buffer) {
        alDeleteBuffers(1, &buffer);
    }
}

bool AudioBuffer::loadWAV(const std::string& filename) {
    // Simple WAV loading (would need proper implementation)
    // For now, return false - proper WAV loading requires parsing RIFF format
    std::cerr << "WAV loading not fully implemented yet: " << filename << std::endl;
    return false;
}

bool AudioBuffer::loadOGG(const std::string& filename) {
    // OGG/Vorbis loading (would need libvorbis)
    std::cerr << "OGG loading not fully implemented yet: " << filename << std::endl;
    return false;
}

// AudioSource implementation
AudioSource::AudioSource() {
    alGenSources(1, &source);
}

AudioSource::~AudioSource() {
    if (source) {
        alSourceStop(source);
        alDeleteSources(1, &source);
    }
}

void AudioSource::setBuffer(AudioBuffer* buffer) {
    if (buffer && buffer->isLoaded()) {
        alSourcei(source, AL_BUFFER, buffer->getBufferID());
    }
}

void AudioSource::play() {
    alSourcePlay(source);
}

void AudioSource::pause() {
    alSourcePause(source);
}

void AudioSource::stop() {
    alSourceStop(source);
}

void AudioSource::setLooping(bool loop) {
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void AudioSource::setVolume(float volume) {
    alSourcef(source, AL_GAIN, volume);
}

void AudioSource::setPitch(float pitch) {
    alSourcef(source, AL_PITCH, pitch);
}

void AudioSource::setPosition(float x, float y, float z) {
    alSource3f(source, AL_POSITION, x, y, z);
}

void AudioSource::setVelocity(float x, float y, float z) {
    alSource3f(source, AL_VELOCITY, x, y, z);
}

bool AudioSource::isPlaying() const {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

bool AudioSource::isPaused() const {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}

} // namespace BVA
