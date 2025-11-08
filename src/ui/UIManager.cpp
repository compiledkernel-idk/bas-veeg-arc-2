#include "ui/UIManager.hpp"
#include "gameplay/Character.hpp"
#include <iostream>

namespace BVA {

UIManager::UIManager() {}

UIManager::~UIManager() {
    shutdown();
}

bool UIManager::initialize(Ogre::SceneManager* sceneManager) {
    std::cout << "UI manager initializing..." << std::endl;

    overlayManager = Ogre::OverlayManager::getSingletonPtr();
    if (!overlayManager) {
        std::cerr << "Failed to get OverlayManager!" << std::endl;
        return false;
    }

    // Create all UI elements
    createMainMenu();
    createCharacterSelectUI();
    createGameplayHUD();
    createPauseMenu();
    createSettingsMenu();
    createEndScreens();

    // Show main menu by default
    showMainMenu();

    std::cout << "UI manager initialized" << std::endl;
    return true;
}

void UIManager::shutdown() {
    // Cleanup overlays
    if (overlayManager) {
        // Overlays will be cleaned up by Ogre
    }
}

void UIManager::update(float dt) {
    // Update subtitle timer
    if (subtitleActive && subtitleTimer > 0.0f) {
        subtitleTimer -= dt;
        if (subtitleTimer <= 0.0f) {
            hideSubtitles();
        }
    }
}

void UIManager::setState(UIState state) {
    // Hide all overlays first
    if (mainMenuOverlay) mainMenuOverlay->hide();
    if (characterSelectOverlay) characterSelectOverlay->hide();
    if (hudOverlay) hudOverlay->hide();
    if (pauseOverlay) pauseOverlay->hide();
    if (settingsOverlay) settingsOverlay->hide();
    if (victoryOverlay) victoryOverlay->hide();
    if (defeatOverlay) defeatOverlay->hide();
    if (loadingOverlay) loadingOverlay->hide();

    currentState = state;

    // Show appropriate overlay
    switch (state) {
        case UIState::MainMenu:
            if (mainMenuOverlay) mainMenuOverlay->show();
            break;
        case UIState::CharacterSelect:
            if (characterSelectOverlay) characterSelectOverlay->show();
            break;
        case UIState::GameplayHUD:
            if (hudOverlay) hudOverlay->show();
            break;
        case UIState::PauseMenu:
            if (pauseOverlay) pauseOverlay->show();
            break;
        case UIState::SettingsMenu:
            if (settingsOverlay) settingsOverlay->show();
            break;
        case UIState::VictoryScreen:
            if (victoryOverlay) victoryOverlay->show();
            break;
        case UIState::DefeatScreen:
            if (defeatOverlay) defeatOverlay->show();
            break;
        case UIState::Loading:
            if (loadingOverlay) loadingOverlay->show();
            break;
        default:
            break;
    }
}

void UIManager::createMainMenu() {
    std::cout << "Creating main menu UI..." << std::endl;

    try {
        mainMenuOverlay = overlayManager->create("MainMenuOverlay");

        // Create title panel
        Ogre::OverlayContainer* titlePanel = static_cast<Ogre::OverlayContainer*>(
            overlayManager->createOverlayElement("Panel", "MainMenu/TitlePanel"));
        titlePanel->setPosition(0.3f, 0.1f);
        titlePanel->setDimensions(0.4f, 0.2f);
        titlePanel->setMaterialName("UI/TitleBackground");

        // Create title text
        Ogre::TextAreaOverlayElement* titleText = static_cast<Ogre::TextAreaOverlayElement*>(
            overlayManager->createOverlayElement("TextArea", "MainMenu/TitleText"));
        titleText->setPosition(0, 0);
        titleText->setDimensions(1, 1);
        titleText->setCaption("BAS VEEG ARC 3D");
        titleText->setCharHeight(0.05f);
        titleText->setFontName("BlueHighway");
        titleText->setColour(Ogre::ColourValue(1.0f, 1.0f, 0.0f));

        titlePanel->addChild(titleText);
        mainMenuOverlay->add2D(titlePanel);

        std::cout << "Main menu created" << std::endl;
    } catch (const Ogre::Exception& e) {
        std::cerr << "Error creating main menu: " << e.what() << std::endl;
        // Continue without UI - not critical for engine testing
    }
}

void UIManager::createCharacterSelectUI() {
    std::cout << "Creating character select UI..." << std::endl;

    try {
        characterSelectOverlay = overlayManager->create("CharacterSelectOverlay");

        // Create character grid panels
        for (int i = 0; i < 13; i++) {
            std::string panelName = "CharSelect/Character" + std::to_string(i);
            Ogre::OverlayContainer* charPanel = static_cast<Ogre::OverlayContainer*>(
                overlayManager->createOverlayElement("Panel", panelName));

            int row = i / 4;
            int col = i % 4;
            charPanel->setPosition(0.1f + col * 0.2f, 0.2f + row * 0.25f);
            charPanel->setDimensions(0.15f, 0.2f);

            characterSelectOverlay->add2D(charPanel);
        }

        selectedCharacters.resize(4, -1); // Support up to 4 players

        std::cout << "Character select created" << std::endl;
    } catch (const Ogre::Exception& e) {
        std::cerr << "Error creating character select: " << e.what() << std::endl;
    }
}

void UIManager::createGameplayHUD() {
    std::cout << "Creating gameplay HUD..." << std::endl;

    try {
        hudOverlay = overlayManager->create("HUDOverlay");

        // Create player HUDs (support up to 4 players)
        playerHUDs.resize(4);

        for (int i = 0; i < 4; i++) {
            std::string baseName = "HUD/Player" + std::to_string(i);

            // Health bar background
            Ogre::OverlayContainer* healthBg = static_cast<Ogre::OverlayContainer*>(
                overlayManager->createOverlayElement("Panel", baseName + "/HealthBg"));
            healthBg->setPosition(0.05f + i * 0.25f, 0.05f);
            healthBg->setDimensions(0.2f, 0.03f);
            healthBg->setMaterialName("UI/HealthBarBg");

            // Health bar foreground
            Ogre::OverlayContainer* healthBar = static_cast<Ogre::OverlayContainer*>(
                overlayManager->createOverlayElement("Panel", baseName + "/HealthBar"));
            healthBar->setPosition(0, 0);
            healthBar->setDimensions(1.0f, 1.0f);
            healthBar->setMaterialName("UI/HealthBarFg");

            // Ability cooldown indicator
            Ogre::OverlayContainer* abilityCd = static_cast<Ogre::OverlayContainer*>(
                overlayManager->createOverlayElement("Panel", baseName + "/AbilityCooldown"));
            abilityCd->setPosition(0.05f + i * 0.25f, 0.09f);
            abilityCd->setDimensions(0.2f, 0.02f);
            abilityCd->setMaterialName("UI/AbilityCooldown");

            // Health text
            Ogre::TextAreaOverlayElement* healthText = static_cast<Ogre::TextAreaOverlayElement*>(
                overlayManager->createOverlayElement("TextArea", baseName + "/HealthText"));
            healthText->setPosition(0.05f + i * 0.25f, 0.12f);
            healthText->setCharHeight(0.03f);
            healthText->setFontName("BlueHighway");
            healthText->setCaption("100/100");

            playerHUDs[i].healthBar = healthBar;
            playerHUDs[i].abilityCooldown = abilityCd;
            playerHUDs[i].healthText = healthText;

            healthBg->addChild(healthBar);
            hudOverlay->add2D(healthBg);
            hudOverlay->add2D(abilityCd);
            hudOverlay->add2D(healthText);
        }

        std::cout << "Gameplay HUD created" << std::endl;
    } catch (const Ogre::Exception& e) {
        std::cerr << "Error creating HUD: " << e.what() << std::endl;
    }
}

void UIManager::createPauseMenu() {
    std::cout << "Creating pause menu..." << std::endl;

    try {
        pauseOverlay = overlayManager->create("PauseOverlay");

        // Simple pause text
        Ogre::TextAreaOverlayElement* pauseText = static_cast<Ogre::TextAreaOverlayElement*>(
            overlayManager->createOverlayElement("TextArea", "Pause/Text"));
        pauseText->setPosition(0.4f, 0.4f);
        pauseText->setCharHeight(0.1f);
        pauseText->setFontName("BlueHighway");
        pauseText->setCaption("PAUSED");

        pauseOverlay->add2D(pauseText);

        std::cout << "Pause menu created" << std::endl;
    } catch (const Ogre::Exception& e) {
        std::cerr << "Error creating pause menu: " << e.what() << std::endl;
    }
}

void UIManager::createSettingsMenu() {
    std::cout << "Creating settings menu..." << std::endl;

    try {
        settingsOverlay = overlayManager->create("SettingsOverlay");
        std::cout << "Settings menu created" << std::endl;
    } catch (const Ogre::Exception& e) {
        std::cerr << "Error creating settings: " << e.what() << std::endl;
    }
}

void UIManager::createEndScreens() {
    std::cout << "Creating end screens..." << std::endl;

    try {
        // Victory screen
        victoryOverlay = overlayManager->create("VictoryOverlay");
        Ogre::TextAreaOverlayElement* victoryText = static_cast<Ogre::TextAreaOverlayElement*>(
            overlayManager->createOverlayElement("TextArea", "Victory/Text"));
        victoryText->setPosition(0.35f, 0.4f);
        victoryText->setCharHeight(0.1f);
        victoryText->setFontName("BlueHighway");
        victoryText->setCaption("VICTORY!");
        victoryText->setColour(Ogre::ColourValue(0.0f, 1.0f, 0.0f));
        victoryOverlay->add2D(victoryText);

        // Defeat screen
        defeatOverlay = overlayManager->create("DefeatOverlay");
        Ogre::TextAreaOverlayElement* defeatText = static_cast<Ogre::TextAreaOverlayElement*>(
            overlayManager->createOverlayElement("TextArea", "Defeat/Text"));
        defeatText->setPosition(0.35f, 0.4f);
        defeatText->setCharHeight(0.1f);
        defeatText->setFontName("BlueHighway");
        defeatText->setCaption("DEFEAT");
        defeatText->setColour(Ogre::ColourValue(1.0f, 0.0f, 0.0f));
        defeatOverlay->add2D(defeatText);

        std::cout << "End screens created" << std::endl;
    } catch (const Ogre::Exception& e) {
        std::cerr << "Error creating end screens: " << e.what() << std::endl;
    }
}

void UIManager::showMainMenu() {
    setState(UIState::MainMenu);
}

void UIManager::hideMainMenu() {
    if (mainMenuOverlay) mainMenuOverlay->hide();
}

void UIManager::showCharacterSelect() {
    setState(UIState::CharacterSelect);
}

void UIManager::setSelectedCharacter(int playerIndex, int characterIndex) {
    if (playerIndex >= 0 && playerIndex < selectedCharacters.size()) {
        selectedCharacters[playerIndex] = characterIndex;
    }
}

int UIManager::getSelectedCharacter(int playerIndex) const {
    if (playerIndex >= 0 && playerIndex < selectedCharacters.size()) {
        return selectedCharacters[playerIndex];
    }
    return -1;
}

bool UIManager::isCharacterSelectReady() const {
    return selectedCharacters[0] >= 0; // At least player 1 must select
}

void UIManager::showGameplayHUD() {
    setState(UIState::GameplayHUD);
}

void UIManager::hideGameplayHUD() {
    if (hudOverlay) hudOverlay->hide();
}

void UIManager::updatePlayerHealth(int playerIndex, float health, float maxHealth) {
    if (playerIndex < 0 || playerIndex >= playerHUDs.size()) return;

    float percent = health / maxHealth;

    if (playerHUDs[playerIndex].healthBar) {
        playerHUDs[playerIndex].healthBar->setWidth(percent);
    }

    if (playerHUDs[playerIndex].healthText) {
        std::string text = std::to_string((int)health) + "/" + std::to_string((int)maxHealth);
        playerHUDs[playerIndex].healthText->setCaption(text);
    }
}

void UIManager::updatePlayerAbilityCooldown(int playerIndex, float percent) {
    if (playerIndex < 0 || playerIndex >= playerHUDs.size()) return;

    if (playerHUDs[playerIndex].abilityCooldown) {
        playerHUDs[playerIndex].abilityCooldown->setWidth(percent);
    }
}

void UIManager::showComboCounter(int combo) {
    std::cout << "COMBO: " << combo << "x" << std::endl;
    // TODO: Display combo counter on HUD
}

void UIManager::showDamageNumber(float damage, const Ogre::Vector3& position) {
    std::cout << "Damage: " << damage << " at position (" << position.x << ", "
              << position.y << ", " << position.z << ")" << std::endl;
    // TODO: Create floating damage number
}

void UIManager::showPauseMenu() {
    setState(UIState::PauseMenu);
}

void UIManager::hidePauseMenu() {
    if (pauseOverlay) pauseOverlay->hide();
}

void UIManager::showSettings() {
    setState(UIState::SettingsMenu);
}

void UIManager::hideSettings() {
    if (settingsOverlay) settingsOverlay->hide();
}

void UIManager::updateGraphicsSetting(const std::string& setting, int value) {
    std::cout << "Graphics setting '" << setting << "' set to " << value << std::endl;
}

void UIManager::showVictoryScreen(int score, float time) {
    setState(UIState::VictoryScreen);
    std::cout << "Victory! Score: " << score << ", Time: " << time << "s" << std::endl;
}

void UIManager::showDefeatScreen() {
    setState(UIState::DefeatScreen);
}

void UIManager::showSubtitle(const std::string& speaker, const std::string& dutchText,
                              const std::string& englishText, float duration) {
    std::cout << "[" << speaker << "]: " << dutchText << " (" << englishText << ")" << std::endl;

    // TODO: Display on screen
    subtitleActive = true;
    subtitleTimer = duration;
}

void UIManager::hideSubtitles() {
    subtitleActive = false;
    subtitleTimer = 0.0f;
}

void UIManager::showLoadingScreen(const std::string& text) {
    setState(UIState::Loading);
    std::cout << "Loading: " << text << std::endl;
}

void UIManager::updateLoadingProgress(float percent) {
    std::cout << "Loading progress: " << (percent * 100.0f) << "%" << std::endl;
}

void UIManager::hideLoadingScreen() {
    if (loadingOverlay) loadingOverlay->hide();
}

void UIManager::setButtonCallback(const std::string& buttonName, ButtonCallback callback) {
    std::cout << "Button callback registered for: " << buttonName << std::endl;
    // TODO: Implement button click handling
}

} // namespace BVA
