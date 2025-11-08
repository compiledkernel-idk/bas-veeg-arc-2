#include "core/Engine.hpp"
#include "graphics/GraphicsEngine.hpp"
#include "physics/PhysicsEngine.hpp"
#include "audio/AudioEngine.hpp"
#include "core/InputManager.hpp"
#include "core/GameStateManager.hpp"
#include "network/NetworkManager.hpp"
#include <iostream>
#include <thread>

namespace BVA {

Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

bool Engine::initialize() {
    std::cout << "Initializing subsystems..." << std::endl;

    // Initialize graphics first
    graphics = std::make_unique<GraphicsEngine>();
    if (!graphics->initialize()) {
        std::cerr << "Failed to initialize graphics engine!" << std::endl;
        return false;
    }
    std::cout << "  - Graphics engine: OK" << std::endl;

    // Initialize physics
    physics = std::make_unique<PhysicsEngine>();
    if (!physics->initialize()) {
        std::cerr << "Failed to initialize physics engine!" << std::endl;
        return false;
    }
    std::cout << "  - Physics engine: OK" << std::endl;

    // Initialize audio
    audio = std::make_unique<AudioEngine>();
    if (!audio->initialize()) {
        std::cerr << "Failed to initialize audio engine!" << std::endl;
        return false;
    }
    std::cout << "  - Audio engine: OK" << std::endl;

    // Initialize input
    input = std::make_unique<InputManager>();
    if (!input->initialize()) {
        std::cerr << "Failed to initialize input manager!" << std::endl;
        return false;
    }
    std::cout << "  - Input manager: OK" << std::endl;

    // Initialize network
    network = std::make_unique<NetworkManager>();
    if (!network->initialize()) {
        std::cerr << "Failed to initialize network manager!" << std::endl;
        return false;
    }
    std::cout << "  - Network manager: OK" << std::endl;

    // Initialize game state
    gameState = std::make_unique<GameStateManager>();
    if (!gameState->initialize()) {
        std::cerr << "Failed to initialize game state manager!" << std::endl;
        return false;
    }
    std::cout << "  - Game state manager: OK" << std::endl;

    running = true;
    return true;
}

void Engine::run() {
    auto previousTime = std::chrono::high_resolution_clock::now();
    float accumulator = 0.0f;

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - previousTime;
        previousTime = currentTime;

        float frameTime = elapsed.count();
        // Cap frame time to prevent spiral of death
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        accumulator += frameTime;

        // Fixed timestep updates
        int updateCount = 0;
        while (accumulator >= FIXED_TIMESTEP && updateCount < MAX_FRAME_SKIP) {
            update(FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
            updateCount++;
        }

        // Variable timestep rendering
        deltaTime = frameTime;
        render();

        frameCount++;

        // Cap framerate to ~240 FPS
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void Engine::update(float dt) {
    // Update input first
    input->update(dt);

    // Check for quit
    if (input->isKeyPressed(KeyCode::Escape) &&
        gameState->getState() == GameState::MainMenu) {
        quit();
        return;
    }

    // Update network
    network->update(dt);

    // Update game state
    gameState->update(dt);

    // Update physics
    physics->update(dt);

    // Update audio
    audio->update(dt);

    // Update graphics (animations, particles, etc.)
    graphics->update(dt);
}

void Engine::render() {
    graphics->render();
}

void Engine::shutdown() {
    std::cout << "Shutting down subsystems..." << std::endl;

    if (gameState) {
        gameState->shutdown();
        gameState.reset();
        std::cout << "  - Game state manager: Shutdown" << std::endl;
    }

    if (network) {
        network->shutdown();
        network.reset();
        std::cout << "  - Network manager: Shutdown" << std::endl;
    }

    if (input) {
        input->shutdown();
        input.reset();
        std::cout << "  - Input manager: Shutdown" << std::endl;
    }

    if (audio) {
        audio->shutdown();
        audio.reset();
        std::cout << "  - Audio engine: Shutdown" << std::endl;
    }

    if (physics) {
        physics->shutdown();
        physics.reset();
        std::cout << "  - Physics engine: Shutdown" << std::endl;
    }

    if (graphics) {
        graphics->shutdown();
        graphics.reset();
        std::cout << "  - Graphics engine: Shutdown" << std::endl;
    }

    running = false;
}

} // namespace BVA
