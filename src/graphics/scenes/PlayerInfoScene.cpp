#include "graphics/scenes/PlayerInfoScene.hpp"
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

    auto nameLabelUPtr = std::make_unique<UILabel>(Vector2{1000, 300}, "Enter your Name:", 30, WHITE);
    nameLabelUPtr->setFont(font);
    auto timeLabelUPtr = std::make_unique<UILabel>(Vector2{910, 500}, "Enter the last time you ate garlic:", 30, WHITE);
    timeLabelUPtr->setFont(font);

    auto continueButtonUPtr = std::make_unique<UIButton>(Rectangle{1050, 700, 100, 50}, "Submit", 30, WHITE, GRAY, DARKGRAY, WHITE);
    continueButton = continueButtonUPtr.get();

    auto backButtonUPtr = std::make_unique<UIButton>(Rectangle{40, 800, 80, 50}, "Back", 30, BLACK, LIGHTGRAY);
    backButton = backButtonUPtr.get();

    continueButton->setFont(font);
    backButton->setFont(font);

    uiManager.add(std::move(nameInputUPtr));
    uiManager.add(std::move(timeInputUPtr));
    uiManager.add(std::move(nameLabelUPtr));
    uiManager.add(std::move(timeLabelUPtr));
    uiManager.add(std::move(continueButtonUPtr));
    uiManager.add(std::move(backButtonUPtr));

    updateBackButtonVisibility();

    continueButton->setOnClick([this]() {
        std::string name = nameInput->getText();
        std::string time = timeInput->getText();

        if (name.empty() || time.empty())
        {
            showErrorMessage("Pleas enter all seaction!");
            return;
        }

        playerNames.push_back(name);
        playerTimes.push_back(time);

        if (currentPlayer == 1)
        {
            currentPlayer = 2;
            resetInputsForNextPlayer();
            updateBackButtonVisibility();
        }
        else
        {
            SceneManager::getInstance().goTo("NextScene");
        }
    });

    backButton->setOnClick([this]() {
        if (currentPlayer == 2)
        {
            if (!playerNames.empty()) playerNames.pop_back();
            if (!playerTimes.empty()) playerTimes.pop_back();
            currentPlayer = 1;
            resetInputsForNextPlayer();
            updateBackButtonVisibility();
        }
    });
}


PlayerInfoScene::~PlayerInfoScene()
{
    UnloadTexture(backgroundTexture);
    UnloadFont(font);
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

void PlayerInfoScene::showErrorMessage(const std::string &msg)
{
    errorMessage = msg;
    errorDisplayTime = 3.0f;
}

void PlayerInfoScene::updateBackButtonVisibility()
{
    if (currentPlayer == 2)
        backButton->setVisible(true);
    else
        backButton->setVisible(false);
}

void PlayerInfoScene::onEnter()
{
    resetInputsForNextPlayer();
    updateBackButtonVisibility();
}

void PlayerInfoScene::onExit()
{
}

void PlayerInfoScene::update(float deltaTime)
{
    uiManager.update();

    if (IsKeyPressed(KEY_ENTER))
    {
        continueButton->click();
    }
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        SceneManager::getInstance().goTo("MainMenu");
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
        Rectangle{ 0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height },
        Rectangle{ 0, 0, 1600, 900 },
        Vector2{ 0, 0 },
        0.0f,
        WHITE
    );

    uiManager.render();

    if (!errorMessage.empty())
    {
        if (errorDisplayTime > 0.0f)
    {
        DrawTextEx(
            font, 
            errorMessage.c_str(), 
            {600, 40},
            40,
            2,                                 // فاصله بین حروف
            RED 
        );
    }
    }
}
