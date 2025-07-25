#include "graphics/scenes/PlayerInfoScene.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "core/Game.hpp"
#include <iostream>

void PlayerInfoScene::onEnter()
{
    backgroundTexture = TextureManager::getInstance().getOrLoadTexture("PlayerInfo", "assets/images/background/player_info.png");

    font = LoadFontEx("assets/fonts/simple.ttf", 30, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    fontLoaded = true;

    currentPlayer = 1;
    uiManager.clear();
    continueButton = nullptr;
    backButton = nullptr;
    nameInput = nullptr;
    timeInput = nullptr;
    playerLabel = nullptr;

    createButtons();
    createInputBoxes();
    createLabels();

    resetInputsForNextPlayer();
    debugCheckUIElements();
}

void PlayerInfoScene::onExit()
{

    continueButton = nullptr;
    backButton = nullptr;
    nameInput = nullptr;
    timeInput = nullptr;
    playerLabel = nullptr;

    UnloadFont(font);
    fontLoaded = false;
    uiManager.clear();
}

void PlayerInfoScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    uiManager.update();
}

void PlayerInfoScene::render()
{
    ClearBackground(BLACK);
    DrawTexturePro(backgroundTexture,
                   Rectangle{0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0},
                   0.0f,
                   WHITE);

    uiManager.render();
}

void PlayerInfoScene::resetInputsForNextPlayer()
{
    if (nameInput)
        nameInput->setText("");
    if (timeInput)
        timeInput->setText("");
}

void PlayerInfoScene::showErrorMessage(const std::string &msg)
{
    std::cout << "[DEBUG] showErrorMg" << msg << '\n';
    errorLabel->setText(msg);
}

void PlayerInfoScene::createButtons()
{
    auto continueButtonUPtr = std::make_unique<UIButton>(
        Rectangle{1030, 700, 150, 50}, "SUBMIT", 30, WHITE, DARKGRAY, GRAY, WHITE);
    continueButtonUPtr->setFont(font);
    continueButton = continueButtonUPtr.get();

    auto backButtonUPtr = std::make_unique<UIButton>(
        Rectangle{40, 800, 200, 50}, "Main Menu", 30, WHITE, DARKGRAY, GRAY, WHITE);
    backButtonUPtr->setFont(font);
    backButton = backButtonUPtr.get();

    uiManager.add(std::move(continueButtonUPtr));
    uiManager.add(std::move(backButtonUPtr));

    continueButton->setOnClick([this]()
                               {
    AudioManager::getInstance().playSoundEffect("click");

    if (!nameInput || !timeInput) {
        showErrorMessage("Internal error: input boxes not ready.");
        return;
    }

    std::string name = nameInput->getText();
    std::string time = timeInput->getText();

    if (name.empty() || time.empty())
    {
        if (currentPlayer == 2) 
            return;
        showErrorMessage("Please enter both name and time!");
        return;
    }

    try {
        Game::getInstance().setPlayersTimes(currentPlayer, name, time);
    } catch (const std::invalid_argument &ex) {
        showErrorMessage(ex.what());
        return;
    }

    if (currentPlayer == 1) {
        currentPlayer = 2;
        if (playerLabel) playerLabel->setText("Player 2 entering info");
        resetInputsForNextPlayer();
    } else {
        SceneManager::getInstance().goTo(SceneKeys::HERO_SELECTION_SCENE);
    } });

    backButton->setOnClick([this]()
                           {
        AudioManager::getInstance().playSoundEffect("click");
        currentPlayer = 1;
        if (playerLabel) playerLabel->setText("Player 1 entering info");
        resetInputsForNextPlayer();
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });
}

void PlayerInfoScene::createInputBoxes()
{
    auto nameInputUPtr = std::make_unique<UIInputBox>(
        Rectangle{900, 350, 400, 60}, 30, 40, GRAY, WHITE, BLACK);
    nameInputUPtr->setFont(font);
    nameInput = nameInputUPtr.get();

    auto timeInputUPtr = std::make_unique<UIInputBox>(
        Rectangle{900, 550, 400, 60}, 30, 40, GRAY, WHITE, BLACK);
    timeInputUPtr->setFont(font);
    timeInput = timeInputUPtr.get();

    uiManager.add(std::move(nameInputUPtr));
    uiManager.add(std::move(timeInputUPtr));
}

void PlayerInfoScene::createLabels()
{
    auto playerLabelUPtr = std::make_unique<UILabel>(
        Vector2{600, 40}, "Player 1 entering info", 60, 0.0f, WHITE);
    playerLabelUPtr->setFont(font);
    playerLabel = playerLabelUPtr.get();
    uiManager.add(std::move(playerLabelUPtr));

    Vector2 namePos = {900, 300};
    Vector2 timePos = {900, 500};

    auto nameLabelUPtr = std::make_unique<UILabel>(
        namePos, "Enter your Name:", 30, 0.0f, WHITE);
    nameLabelUPtr->setFont(font);

    auto timeLabelUPtr = std::make_unique<UILabel>(
        timePos, "Enter the last time you ate garlic:", 30, 0.0f, WHITE);
    timeLabelUPtr->setFont(font);

    uiManager.add(std::move(nameLabelUPtr));
    uiManager.add(std::move(timeLabelUPtr));

    auto errorLabelUPtr = std::make_unique<UILabel>(
        Vector2{450, 700}, "", 40, 3.0f, RED);
    errorLabelUPtr->setFont(font);
    errorLabel = errorLabelUPtr.get();
    uiManager.add(std::move(errorLabelUPtr));
}

void PlayerInfoScene::debugCheckUIElements()
{
    std::cout << "[DEBUG] Checking UI elements..." << std::endl;
    std::cout << "[DEBUG] continueButton: " << (continueButton ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] backButton: " << (backButton ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] nameInput: " << (nameInput ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] timeInput: " << (timeInput ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] playerLabel: " << (playerLabel ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] errorLabel: " << (errorLabel ? "OK" : "NULL") << std::endl;
}
