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
    std::cout << "[DEBUG] PlayerInfoScene::onEnter()" << std::endl;

    if (font.texture.id == 0)
        std::cout << "[DEBUG] Font texture is invalid!" << std::endl;

    currentPlayer = 1;
    uiManager.clear();
    continueButton = nullptr;
    backButton = nullptr;
    nameInput = nullptr;
    timeInput = nullptr;
    playerLabel = nullptr;

    backgroundTexture = TextureManager::getInstance().getOrLoadTexture("PlayerInfo", "assets/images/background/player_info.png");

    font = LoadFont("assets/fonts/simple.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    createButtons();
    createInputBoxes();
    createLabels(); // شامل playerLabel

    errorMessage.clear();
    errorDisplayTime = 0.0f;

    resetInputsForNextPlayer();
    debugCheckUIElements();
}

void PlayerInfoScene::onExit()
{
    std::cout << "[DEBUG] PlayerInfoScene::onExit()" << std::endl;
    resetInputsForNextPlayer();
    continueButton = nullptr;
    backButton = nullptr;
    nameInput = nullptr;
    timeInput = nullptr;
    playerLabel = nullptr;
    uiManager.clear();
}

void PlayerInfoScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    uiManager.update();

    if (IsKeyPressed(KEY_ENTER))
    {
        // اگر فوکوس روی هر دکمه است، فقط کلیک همان دکمه را اجرا کن
        if (backButton && backButton->isFocus())
        {
            backButton->click();
            return;
        }
        if (continueButton && continueButton->isFocus())
        {
            continueButton->click();
            return;
        }
    }

    // مدیریت پیام خطا
    if (errorDisplayTime > 0.0f)
    {
        errorDisplayTime -= deltaTime;
        if (errorDisplayTime <= 0.0f)
            errorMessage.clear();
    }
}

void PlayerInfoScene::render()
{
    DrawTexturePro(backgroundTexture,
                   Rectangle{0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0},
                   0.0f,
                   WHITE);

    uiManager.render();

    if (!errorMessage.empty())
        DrawTextEx(font, errorMessage.c_str(), {600, 40}, 40, 2, RED);
}

void PlayerInfoScene::resetInputsForNextPlayer()
{
    std::cout << "[DEBUG] resetInputsForNextPlayer()" << std::endl;
    if (nameInput)
        nameInput->setText("");
    if (timeInput)
        timeInput->setText("");
}

void PlayerInfoScene::showErrorMessage(const std::string &msg)
{
    std::cout << "[DEBUG] Error: " << msg << std::endl;
    errorMessage = msg;
    errorDisplayTime = 3.0f;
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

    // اگر در حال تغییر بازیکن هستیم (فریم اول) ارور نده
    if (name.empty() || time.empty())
    {
        if (currentPlayer == 2)   // وقتی می‌خواهد بازیکن دوم را بگیرد
            return;               // ارور نده، فقط بگذار کاربر وارد کند
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
        Vector2{650, 40}, "Player 1 entering info", 40, 0.0f, WHITE);
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
}

void PlayerInfoScene::debugCheckUIElements()
{
    std::cout << "[DEBUG] Checking UI elements..." << std::endl;
    std::cout << "[DEBUG] continueButton: " << (continueButton ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] backButton: " << (backButton ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] nameInput: " << (nameInput ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] timeInput: " << (timeInput ? "OK" : "NULL") << std::endl;
    std::cout << "[DEBUG] playerLabel: " << (playerLabel ? "OK" : "NULL") << std::endl;
}
