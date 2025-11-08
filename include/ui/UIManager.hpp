#pragma once

#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <string>
#include <memory>
#include <functional>
#include <vector>

namespace BVA {

class Character;

enum class UIState {
    MainMenu,
    CharacterSelect,
    GameplayHUD,
    PauseMenu,
    SettingsMenu,
    VictoryScreen,
    DefeatScreen,
    Loading,
    Cutscene
};

class UIManager {
public:
    UIManager();
    ~UIManager();

    bool initialize(Ogre::SceneManager* sceneManager);
    void shutdown();
    void update(float dt);

    // State management
    void setState(UIState state);
    UIState getState() const { return currentState; }

    // Main menu
    void showMainMenu();
    void hideMainMenu();

    // Character select
    void showCharacterSelect();
    void setSelectedCharacter(int playerIndex, int characterIndex);
    int getSelectedCharacter(int playerIndex) const;
    bool isCharacterSelectReady() const;

    // Gameplay HUD
    void showGameplayHUD();
    void hideGameplayHUD();
    void updatePlayerHealth(int playerIndex, float health, float maxHealth);
    void updatePlayerAbilityCooldown(int playerIndex, float percent);
    void showComboCounter(int combo);
    void showDamageNumber(float damage, const Ogre::Vector3& position);

    // Pause menu
    void showPauseMenu();
    void hidePauseMenu();

    // Settings
    void showSettings();
    void hideSettings();
    void updateGraphicsSetting(const std::string& setting, int value);

    // Victory/Defeat
    void showVictoryScreen(int score, float time);
    void showDefeatScreen();

    // Cutscene subtitles
    void showSubtitle(const std::string& speaker, const std::string& dutchText,
                      const std::string& englishText, float duration);
    void hideSubtitles();

    // Loading screen
    void showLoadingScreen(const std::string& text);
    void updateLoadingProgress(float percent);
    void hideLoadingScreen();

    // Input callbacks
    using ButtonCallback = std::function<void()>;
    void setButtonCallback(const std::string& buttonName, ButtonCallback callback);

private:
    void createMainMenu();
    void createCharacterSelectUI();
    void createGameplayHUD();
    void createPauseMenu();
    void createSettingsMenu();
    void createEndScreens();

    Ogre::OverlayManager* overlayManager = nullptr;
    UIState currentState = UIState::MainMenu;

    // Overlays
    Ogre::Overlay* mainMenuOverlay = nullptr;
    Ogre::Overlay* characterSelectOverlay = nullptr;
    Ogre::Overlay* hudOverlay = nullptr;
    Ogre::Overlay* pauseOverlay = nullptr;
    Ogre::Overlay* settingsOverlay = nullptr;
    Ogre::Overlay* victoryOverlay = nullptr;
    Ogre::Overlay* defeatOverlay = nullptr;
    Ogre::Overlay* loadingOverlay = nullptr;
    Ogre::Overlay* subtitleOverlay = nullptr;

    // Character selection
    std::vector<int> selectedCharacters;
    int numPlayers = 1;

    // HUD elements
    struct PlayerHUDElements {
        Ogre::OverlayContainer* healthBar;
        Ogre::OverlayContainer* abilityCooldown;
        Ogre::TextAreaOverlayElement* healthText;
    };
    std::vector<PlayerHUDElements> playerHUDs;

    // Subtitle state
    float subtitleTimer = 0.0f;
    bool subtitleActive = false;
};

} // namespace BVA
