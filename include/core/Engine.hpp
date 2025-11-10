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

#include <memory>
#include <chrono>
#include <OGRE/Ogre.h>

namespace BVA {

class GraphicsEngine;
class PhysicsEngine;
class AudioEngine;
class InputManager;
class GameStateManager;
class NetworkManager;

class Engine {
public:
    static Engine& getInstance();

    bool initialize();
    void run();
    void shutdown();

    // Subsystem accessors
    GraphicsEngine* getGraphics() { return graphics.get(); }
    PhysicsEngine* getPhysics() { return physics.get(); }
    AudioEngine* getAudio() { return audio.get(); }
    InputManager* getInput() { return input.get(); }
    GameStateManager* getGameState() { return gameState.get(); }
    NetworkManager* getNetwork() { return network.get(); }

    float getDeltaTime() const { return deltaTime; }
    uint64_t getFrameCount() const { return frameCount; }
    bool isRunning() const { return running; }
    void quit() { running = false; }

private:
    Engine() = default;
    ~Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    void update(float dt);
    void render();

    std::unique_ptr<GraphicsEngine> graphics;
    std::unique_ptr<PhysicsEngine> physics;
    std::unique_ptr<AudioEngine> audio;
    std::unique_ptr<InputManager> input;
    std::unique_ptr<GameStateManager> gameState;
    std::unique_ptr<NetworkManager> network;

    bool running = false;
    float deltaTime = 0.0f;
    uint64_t frameCount = 0;

    // Fixed timestep
    static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
    static constexpr int MAX_FRAME_SKIP = 5;
};

} // namespace BVA
