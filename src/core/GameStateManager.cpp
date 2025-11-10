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


#include "core/GameStateManager.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace BVA {

GameStateManager::GameStateManager() {}

GameStateManager::~GameStateManager() {
    shutdown();
}

bool GameStateManager::initialize() {
    setupStoryLevels();
    std::cout << "Game state manager initialized" << std::endl;
    std::cout << "  Story levels: " << storyLevels.size() << std::endl;
    return true;
}

void GameStateManager::shutdown() {
    players.clear();
    enemies.clear();
    currentBoss.reset();
}

void GameStateManager::update(float dt) {
    if (currentState == GameState::InGame || currentState == GameState::BossFight) {
        playTime += dt;

        // Update combat logic
        updateCombatLogic(dt);

        // Update combo timer
        updateCombo(dt);

        // Update players
        for (auto& player : players) {
            if (player) {
                player->update(dt);
            }
        }

        // Update enemies
        for (auto& enemy : enemies) {
            if (enemy) {
                enemy->update(dt);
            }
        }

        // Update boss
        if (currentBoss) {
            currentBoss->update(dt);
        }

        // Check win/lose conditions
        checkVictoryCondition();
        checkDefeatCondition();
    }

    if (matchActive) {
        matchTime += dt;
    }
}

void GameStateManager::setState(GameState state) {
    GameState previousState = currentState;
    currentState = state;

    std::cout << "Game state changed: " << static_cast<int>(previousState)
              << " -> " << static_cast<int>(currentState) << std::endl;

    // Handle state transitions
    if (state == GameState::InGame && previousState != GameState::Paused) {
        matchActive = true;
        matchTime = 0.0f;
    } else if (state == GameState::Victory || state == GameState::Defeat) {
        matchActive = false;
    }
}

void GameStateManager::setGameMode(GameMode mode) {
    currentGameMode = mode;
    std::cout << "Game mode set to: " << static_cast<int>(mode) << std::endl;
}

void GameStateManager::startStoryMode() {
    setGameMode(GameMode::StoryMode);
    currentLevel = 0;
    loadLevel(0);
}

void GameStateManager::loadLevel(int levelIndex) {
    if (levelIndex < 0 || levelIndex >= storyLevels.size()) {
        std::cerr << "Invalid level index: " << levelIndex << std::endl;
        return;
    }

    cleanupLevel();
    currentLevel = levelIndex;

    const LevelData& level = storyLevels[levelIndex];
    std::cout << "Loading level: " << level.name << std::endl;

    // Play intro cutscene if available
    if (!level.cutsceneBefore.empty()) {
        playCutscene(level.cutsceneBefore);
    } else {
        setState(GameState::InGame);
    }

    // TODO: Load actual level geometry and spawn enemies
}

void GameStateManager::completeLevel() {
    if (currentLevel >= storyLevels.size()) return;

    const LevelData& level = storyLevels[currentLevel];

    // Play outro cutscene if available
    if (!level.cutsceneAfter.empty()) {
        playCutscene(level.cutsceneAfter);
    }

    // Progress to next level
    nextLevel();
}

void GameStateManager::nextLevel() {
    currentLevel++;
    if (currentLevel >= storyLevels.size()) {
        // Story complete!
        std::cout << "Story mode complete!" << std::endl;
        setState(GameState::Victory);
    } else {
        loadLevel(currentLevel);
    }
}

bool GameStateManager::isStoryComplete() const {
    return currentLevel >= storyLevels.size();
}

void GameStateManager::startVersus(bool online) {
    setGameMode(online ? GameMode::OnlineVersus : GameMode::VersusLocal);
    setState(GameState::InGame);
    std::cout << "Starting versus mode (online: " << online << ")" << std::endl;
}

void GameStateManager::startCoop(bool online) {
    setGameMode(online ? GameMode::OnlineCoop : GameMode::CoopLocal);
    setState(GameState::InGame);
    std::cout << "Starting co-op mode (online: " << online << ")" << std::endl;
}

void GameStateManager::endMatch() {
    matchActive = false;
    setState(GameState::MainMenu);
}

void GameStateManager::addPlayer(CharacterID characterId, int playerIndex) {
    if (playerIndex < 0) return;

    // Ensure vector is large enough
    if (playerIndex >= players.size()) {
        players.resize(playerIndex + 1);
    }

    // Create character
    auto character = createCharacter(characterId);
    if (character) {
        // Initialize with engine systems
        auto* engine = &Engine::getInstance();
        character->initialize(engine->getGraphics()->getSceneManager(),
                              engine->getPhysics());

        // Position character
        character->setPosition(Ogre::Vector3(playerIndex * 2.0f, 2.0f, 0.0f));

        players[playerIndex] = std::move(character);
        std::cout << "Added player " << playerIndex << std::endl;
    }
}

void GameStateManager::removePlayer(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < players.size()) {
        if (players[playerIndex]) {
            players[playerIndex]->cleanup();
            players[playerIndex].reset();
        }
    }
}

Character* GameStateManager::getPlayer(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < players.size()) {
        return players[playerIndex].get();
    }
    return nullptr;
}

void GameStateManager::spawnEnemy(const std::string& enemyType, const Ogre::Vector3& position) {
    // TODO: Create enemy based on type
    std::cout << "Spawning enemy: " << enemyType << " at position ("
              << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void GameStateManager::removeEnemy(Character* enemy) {
    auto it = std::find_if(enemies.begin(), enemies.end(),
                            [enemy](const std::unique_ptr<Character>& e) {
                                return e.get() == enemy;
                            });
    if (it != enemies.end()) {
        (*it)->cleanup();
        enemies.erase(it);
    }
}

void GameStateManager::spawnBoss(BossType bossType) {
    currentBoss = createBoss(bossType);
    if (currentBoss) {
        auto* engine = &Engine::getInstance();
        currentBoss->initialize(engine->getGraphics()->getSceneManager(),
                                engine->getPhysics());
        currentBoss->setPosition(Ogre::Vector3(0.0f, 2.0f, 10.0f));
        currentBoss->startBattle();

        setState(GameState::BossFight);
        std::cout << "Boss spawned: " << currentBoss->getName() << std::endl;
        std::cout << currentBoss->getIntroText() << std::endl;
    }
}

void GameStateManager::playCutscene(const std::string& cutsceneName) {
    setState(GameState::Cutscene);
    std::cout << "Playing cutscene: " << cutsceneName << std::endl;
    // TODO: Actually play cutscene
}

void GameStateManager::skipCutscene() {
    if (currentState == GameState::Cutscene) {
        setState(GameState::InGame);
    }
}

void GameStateManager::pauseGame() {
    if (currentState == GameState::InGame || currentState == GameState::BossFight) {
        setState(GameState::Paused);
    }
}

void GameStateManager::resumeGame() {
    if (currentState == GameState::Paused) {
        setState(currentBoss ? GameState::BossFight : GameState::InGame);
    }
}

void GameStateManager::restartLevel() {
    loadLevel(currentLevel);
}

void GameStateManager::quitToMenu() {
    cleanupLevel();
    setState(GameState::MainMenu);
}

void GameStateManager::addScore(int points) {
    totalScore += points;
}

void GameStateManager::incrementCombo() {
    comboCounter++;
    comboTimer = COMBO_TIMEOUT;
}

void GameStateManager::resetCombo() {
    comboCounter = 0;
    comboTimer = 0.0f;
}

void GameStateManager::setupStoryLevels() {
    // Define all story mode levels
    storyLevels = {
        {"The Classroom", "classroom_scene", {"Wolters", "PrefectA"}, BossType::Bastiaan, "intro", ""},
        {"The Cafeteria", "cafeteria_scene", {"Chef", "PrefectB"}, BossType::Mees, "", ""},
        {"The Gym", "gym_scene", {"Coach"}, BossType::HeadChef, "", ""},
        {"The Library", "library_scene", {"Librarian"}, BossType::PrincipalVanDerBerg, "", ""},
        {"The Janitor's Domain", "janitor_scene", {}, BossType::JanitorKing, "", ""},
        {"Final Confrontation", "final_scene", {}, BossType::KeizerBomTaha, "", "ending"}
    };
}

void GameStateManager::cleanupLevel() {
    enemies.clear();
    currentBoss.reset();
    totalScore = 0;
    comboCounter = 0;
    comboTimer = 0.0f;
}

void GameStateManager::updateCombatLogic(float dt) {
    // TODO: Implement combat logic
    // - Check for attacks hitting enemies
    // - Process damage
    // - Check for enemy deaths
    // - Award score/combo
}

void GameStateManager::updateCombo(float dt) {
    if (comboTimer > 0.0f) {
        comboTimer -= dt;
        if (comboTimer <= 0.0f) {
            resetCombo();
        }
    }
}

void GameStateManager::checkVictoryCondition() {
    if (currentState != GameState::InGame && currentState != GameState::BossFight) return;

    // Check if all enemies and boss are defeated
    bool allEnemiesDefeated = enemies.empty();
    bool bossDefeated = !currentBoss || !currentBoss->isAlive();

    if (allEnemiesDefeated && bossDefeated) {
        if (currentGameMode == GameMode::StoryMode) {
            completeLevel();
        } else {
            setState(GameState::Victory);
        }
    }
}

void GameStateManager::checkDefeatCondition() {
    if (currentState != GameState::InGame && currentState != GameState::BossFight) return;

    // Check if all players are defeated
    bool allPlayersDead = true;
    for (const auto& player : players) {
        if (player && player->isAlive()) {
            allPlayersDead = false;
            break;
        }
    }

    if (allPlayersDead) {
        setState(GameState::Defeat);
    }
}

} // namespace BVA
