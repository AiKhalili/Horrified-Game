#include "graphics/scenes/EndGameScene.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "audio/AudioManager.hpp"

void EndGameScene::onEnter()
{
    finalState = Game::getInstance().getGameState();
    loadResources();
    createUI();
}

void EndGameScene::loadResources()
{
    switch (finalState)
    {
    case GameState::GameWon:
        background = TextureManager::getInstance().getOrLoadTexture("win", "assets/images/background/win.png");

        buttonBackgroundColor = Color{120, 0, 0, 255};
        buttonHoverColor = Color{40, 0, 0, 180};
        CadreColor = Color{200, 0, 0, 255};
        buttonTextColor = WHITE;
        AudioManager::getInstance().playWinMusic();
        break;
    case GameState::GameLost:
        background = TextureManager::getInstance().getOrLoadTexture("lose", "assets/images/background/lose.png");
        buttonBackgroundColor = Color{120, 0, 0, 255};
        buttonHoverColor = Color{40, 0, 0, 180};
        CadreColor = Color{200, 0, 0, 255};
        buttonTextColor = WHITE;
        AudioManager::getInstance().playLoseMusic();
        break;
    case GameState::GameOutOfTime:
        background = TextureManager::getInstance().getOrLoadTexture("timeout", "assets/images/background/timeout.png");
        buttonBackgroundColor = Color{245, 245, 220, 255};
        buttonHoverColor = GRAY;
        CadreColor = Color{200, 180, 150, 255};
        buttonTextColor = Color{101, 67, 33, 255};
        AudioManager::getInstance().playTimeOutMusic();
        break;
    default:
        break;
    }
    font = LoadFontEx("assets/fonts/simple.ttf", 70, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void EndGameScene::createUI()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    float btnWidth = 200;
    float btnHeight = 60;
    float margin = 20;
    float x = screenW - btnWidth - 40;

    Rectangle exitBounds = {x, screenH - btnHeight * 2 - margin * 2, btnWidth, btnHeight};

    auto exitBtn = std::make_unique<UIButton>(
        exitBounds, "Exit", 33, buttonTextColor,
        buttonBackgroundColor, buttonHoverColor, CadreColor);
    exitBtn->setFont(font);
    exitBtn->setOnClick([]
                        {
                    AudioManager::getInstance().playSoundEffect("click");
                    SceneManager::getInstance().requestExit(); });

    uiManager.add(std::move(exitBtn));

    Rectangle menuBounds = {x, screenH - btnHeight - margin, btnWidth, btnHeight};
    auto menuBtn = std::make_unique<UIButton>(
        menuBounds, "Main Menu", 33, buttonTextColor,
        buttonBackgroundColor, buttonHoverColor, CadreColor);
    menuBtn->setFont(font);
    menuBtn->setOnClick([]
                        {
                    AudioManager::getInstance().playSoundEffect("click");
                    Game::getInstance().reset();
                    SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });
    uiManager.add(std::move(menuBtn));
}

void EndGameScene::onExit()
{
    UnloadFont(font);
    uiManager.clear();
}

void EndGameScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    uiManager.update();
}

void EndGameScene::render()
{
    ClearBackground(BLACK);
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    DrawTexturePro(
        background,
        {0, 0, (float)background.width, (float)background.height},
        {0, 0, (float)screenW, (float)screenH},
        {0, 0}, 0.0f, WHITE);

    uiManager.render();
}