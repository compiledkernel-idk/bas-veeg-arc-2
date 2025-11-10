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
