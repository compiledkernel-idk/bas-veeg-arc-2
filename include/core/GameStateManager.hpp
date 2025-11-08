#pragma once

#include <memory>
#include <vector>
#include <string>
#include "gameplay/Character.hpp"
#include "gameplay/Boss.hpp"

namespace BVA {

enum class GameMode {
    None,
    StoryMode,
    VersusLocal,
    CoopLocal,
    OnlineVersus,
    OnlineCoop
};

enum class GameState {
    MainMenu,
    CharacterSelect,
    Loading,
    Cutscene,
    InGame,
    BossFight,
    Paused,
    Victory,
    Defeat
};

struct LevelData {
    std::string name;
    std::string sceneName;
    std::vector<std::string> enemies;
    BossType bossType;
    std::string cutsceneBefore;
    std::string cutsceneAfter;
};

class GameStateManager {
public:
    GameStateManager();
    ~GameStateManager();

    bool initialize();
    void shutdown();
    void update(float dt);

    // State management
    void setState(GameState state);
    GameState getState() const { return currentState; }
    void setGameMode(GameMode mode);
    GameMode getGameMode() const { return currentGameMode; }

    // Story mode
    void startStoryMode();
    void loadLevel(int levelIndex);
    void completeLevel();
    void nextLevel();
    int getCurrentLevel() const { return currentLevel; }
    bool isStoryComplete() const;

    // Versus/Coop
    void startVersus(bool online);
    void startCoop(bool online);
    void endMatch();

    // Player management
    void addPlayer(CharacterID characterId, int playerIndex);
    void removePlayer(int playerIndex);
    Character* getPlayer(int playerIndex);
    int getPlayerCount() const { return players.size(); }

    // Enemy management
    void spawnEnemy(const std::string& enemyType, const Ogre::Vector3& position);
    void removeEnemy(Character* enemy);
    const std::vector<std::unique_ptr<Character>>& getEnemies() const { return enemies; }

    // Boss management
    void spawnBoss(BossType bossType);
    Boss* getCurrentBoss() { return currentBoss.get(); }
    bool isBossFight() const { return currentBoss != nullptr; }

    // Cutscenes
    void playCutscene(const std::string& cutsceneName);
    void skipCutscene();
    bool isInCutscene() const { return currentState == GameState::Cutscene; }

    // Game state
    void pauseGame();
    void resumeGame();
    void restartLevel();
    void quitToMenu();

    // Score and stats
    void addScore(int points);
    int getScore() const { return totalScore; }
    float getPlayTime() const { return playTime; }
    int getCombo() const { return comboCounter; }
    void incrementCombo();
    void resetCombo();

private:
    void setupStoryLevels();
    void cleanupLevel();
    void updateCombatLogic(float dt);
    void updateCombo(float dt);
    void checkVictoryCondition();
    void checkDefeatCondition();

    GameState currentState = GameState::MainMenu;
    GameMode currentGameMode = GameMode::None;

    // Story mode data
    std::vector<LevelData> storyLevels;
    int currentLevel = 0;

    // Players and enemies
    std::vector<std::unique_ptr<Character>> players;
    std::vector<std::unique_ptr<Character>> enemies;
    std::unique_ptr<Boss> currentBoss;

    // Game stats
    int totalScore = 0;
    float playTime = 0.0f;
    int comboCounter = 0;
    float comboTimer = 0.0f;
    static constexpr float COMBO_TIMEOUT = 3.0f;

    // Match state
    bool matchActive = false;
    float matchTime = 0.0f;
};

} // namespace BVA
