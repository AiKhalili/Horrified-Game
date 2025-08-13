#include "graphics/scenes/MainMenuScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "core/Game.hpp"

void MainMenuScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("main", "assets/images/background/main_menu.png");
    normalFont = LoadFont("assets/fonts/simple.ttf");
    AudioManager::getInstance().playMusic();
    float x = GetScreenWidth() / 2 - 200;
    float startY = 600;
    float width = 300; // width of button
    float height = 60; // height of button
    float gap = 80;

    // spacing between buttons
    auto makeButton = [&](const std::string &text, int index, std::function<void()> action) { // lambda function with access to outer variables
        Rectangle bounds = {x, startY + gap * index, width, height};
        auto btn = std::make_unique<UIButton>(bounds, text, 32,
                                              WHITE, BLACK, DARKGRAY, Color{239, 224, 197, 255});
        btn->setFont(normalFont);
        btn->setOnClick([action]()
                        {
            AudioManager::getInstance().playSoundEffect("click");
            action(); });
        ui.add(std::move(btn));
    };

    makeButton("Start New Game", 0, std::bind(&MainMenuScene::startGame, this));
    makeButton("Load Game", 1, std::bind(&MainMenuScene::loadGame, this));
    makeButton("Exit", 2, std::bind(&MainMenuScene::exitGame, this));
}

void MainMenuScene::onExit()
{
    UnloadFont(normalFont);
    ui.clear();
}

void MainMenuScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void MainMenuScene::render()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;

    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);
    ui.render();
}

void MainMenuScene::startGame()
{
    SceneManager::getInstance().goTo(SceneKeys::PLAYER_INFO_SCENE);
}

void MainMenuScene::loadGame()
{
    SceneManager::getInstance().goTo(SceneKeys::LOAD_GAMES_SCENE);
}

void MainMenuScene::exitGame()
{
    SceneManager::getInstance().requestExit();
}