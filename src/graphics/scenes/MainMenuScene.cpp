#include "graphics/scenes/MainMenuScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/TextureManager.hpp"

void MainMenuScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("main", "assets/images/background/main_menu.png");
    titleFont = LoadFontEx("assets/fonts/spooky.otf", 100, 0, 0);
    SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);
    normalFont = LoadFont("assets/fonts/simple.ttf");
    AudioManager::getInstance().playMusic();
    float x = GetScreenWidth() / 2 - 150;
    float startY = 600;
    float width = 300; // width of button
    float height = 60; // height of button
    float gap = 80;

    // spacing between buttons
    auto makeButton = [&](const std::string &text, int index, std::function<void()> action) { // lambda function with access to outer variables
        Rectangle bounds = {x, startY + gap * index, width, height};
        auto btn = std::make_unique<UIButton>(bounds, text, 32, WHITE, DARKGRAY, GRAY);
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
    UnloadFont(titleFont);
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
    DrawTexture(background, 0, 0, WHITE);
    Vector2 textSize = MeasureTextEx(titleFont, "HORRIFIED", 100, 2);
    float x = GetScreenWidth() / 2 - textSize.x / 2 - 70;
    float y = 40;
    Color color = {170, 20, 20, 255};
    DrawTextEx(titleFont, "HORRIFIED", Vector2{x, y}, 150, 2, color);
    ui.render();
}

void MainMenuScene::startGame()
{
    SceneManager::getInstance().goTo("BoardScene");
}

void MainMenuScene::loadGame()
{
    SceneManager::getInstance().goTo("SaveSelectScene");
}

void MainMenuScene::exitGame()
{
    CloseWindow();
}