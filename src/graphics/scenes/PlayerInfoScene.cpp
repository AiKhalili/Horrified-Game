#include "graphics/scenes/PlayerInfoScene.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include <iostream>

PlayerInfoScene::PlayerInfoScene()
    : currentPlayer(1)
{
    backgroundTexture = LoadTexture("assets/images/background.png");
    font = LoadFont("assets/fonts/simple.ttf");

    auto nameInputUPtr = std::make_unique<UIInputBox>(Rectangle{900, 350, 400, 60}, 30, 40, GRAY, WHITE, BLACK);
    nameInput = nameInputUPtr.get();
    nameInput->setFont(font);

    auto timeInputUPtr = std::make_unique<UIInputBox>(Rectangle{900, 550, 400, 60}, 30, 40, GRAY, WHITE, BLACK);
    timeInput = timeInputUPtr.get();
    timeInput->setFont(font);

    auto nameLabelUPtr = std::make_unique<UILabel>(Vector2{1000, 300}, "Enter your Name:", 30, 0.0f, WHITE);
    nameLabelUPtr->setFont(font);

    auto timeLabelUPtr = std::make_unique<UILabel>(Vector2{910, 500}, "Enter the last time you ate garlic:", 30, 0.0f, WHITE);
    timeLabelUPtr->setFont(font);

    auto continueButtonUPtr = std::make_unique<UIButton>(Rectangle{1030, 700, 150, 50}, "SUBMIT", 30, WHITE, DARKGRAY, GRAY, WHITE);
    continueButton = continueButtonUPtr.get();
    continueButton->setFont(font);
    continueButton->setFocus(true);

    auto backButtonUPtr = std::make_unique<UIButton>(Rectangle{40, 800, 200, 50}, "Main Menu", 30, WHITE, DARKGRAY, GRAY, WHITE);
    backButton = backButtonUPtr.get();
    backButton->setFont(font);
    backButton->setFocus(true);

    uiManager.add(std::move(nameInputUPtr));
    uiManager.add(std::move(timeInputUPtr));
    uiManager.add(std::move(nameLabelUPtr));
    uiManager.add(std::move(timeLabelUPtr));
    uiManager.add(std::move(continueButtonUPtr));
    uiManager.add(std::move(backButtonUPtr));

    // دکمه SUBMIT
    continueButton->setOnClick([this]() {
        std::string name = nameInput->getText();
        std::string time = timeInput->getText();

        if (name.empty() || time.empty()) {
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
            resetInputsForNextPlayer();
        } else {
            SceneManager::getInstance().goTo(SceneKeys::HERO_SELECTION_SCENE);
        }
    });

    // دکمه بازگشت به منوی اصلی
    backButton->setOnClick([this]() {
        currentPlayer = 1;
        resetInputsForNextPlayer();
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE);
    });
}

PlayerInfoScene::~PlayerInfoScene()
{
    UnloadTexture(backgroundTexture);
    UnloadFont(font);
}

void PlayerInfoScene::resetInputsForNextPlayer()
{
    if (nameInput)
    {
        nameInput->setText("");
        nameInput->setFocus(true);
    }
    if (timeInput)
    {
        timeInput->setText("");
        timeInput->setFocus(false);
    }
}

void PlayerInfoScene::onEnter()
{
    resetInputsForNextPlayer();
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void PlayerInfoScene::onExit()
{
}

void PlayerInfoScene::update(float deltaTime)
{
    uiManager.update();

    if (IsKeyPressed(KEY_ENTER))
    {
        std::string name = nameInput->getText();
        std::string time = timeInput->getText();

        if (name.empty() || time.empty()) return;

        try {
           Game::getInstance().setPlayersTimes(currentPlayer, name, time);
        } catch (const std::invalid_argument &ex) {
            showErrorMessage(ex.what());
            return;
        }

        if (currentPlayer == 1) {
            currentPlayer = 2;
            resetInputsForNextPlayer();
        } else {
            SceneManager::getInstance().goTo(SceneKeys::HERO_SELECTION_SCENE);
        }
    }

    if (errorDisplayTime > 0.0f)
    {
        errorDisplayTime -= deltaTime;
        if (errorDisplayTime <= 0.0f)
        {
            errorMessage = "";
        }
    }
}

void PlayerInfoScene::render()
{
    DrawTexturePro(
        backgroundTexture,
        Rectangle{0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height},
        Rectangle{0, 0, 1600, 900},
        Vector2{0, 0},
        0.0f,
        WHITE
    );

    uiManager.render();

    if (!errorMessage.empty())
    {
        DrawTextEx(font, errorMessage.c_str(), {600, 40}, 40, 2, RED);
    }
}

void PlayerInfoScene::showErrorMessage(const std::string &msg)
{
    errorMessage = msg;
    errorDisplayTime = 3.0f;
}
