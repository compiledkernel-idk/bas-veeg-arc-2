#include "core/Engine.hpp"
#include <iostream>
#include <exception>

int main(int argc, char** argv) {
    try {
        std::cout << "=== Bas Veeg Arc 3D ===" << std::endl;
        std::cout << "Version 1.0.0" << std::endl;
        std::cout << "Initializing..." << std::endl;

        BVA::Engine& engine = BVA::Engine::getInstance();

        if (!engine.initialize()) {
            std::cerr << "Failed to initialize engine!" << std::endl;
            return 1;
        }

        std::cout << "Engine initialized successfully" << std::endl;
        std::cout << "Starting game loop..." << std::endl;

        engine.run();

        std::cout << "Shutting down..." << std::endl;
        engine.shutdown();

        std::cout << "Goodbye!" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return 1;
    }
}
