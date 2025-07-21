#include "graphics/scenes/IntroScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/TextureManager.hpp"

void IntroScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("intro", "assets/images/background/intro_bg.png");
    AudioManager::getInstance().playIntroMusic();
}

void IntroScene::onExit()
{
    AudioManager::getInstance().stopIntroMusic();
}

void IntroScene::update(float deleteTime)
{
    timer += deleteTime;

    AudioManager::getInstance().update();

    if (progress < 1.0f)
    {
        progress += deleteTime * 0.2f;
    }
    if (timer >= 7.0f)
    {
        SceneManager::getInstance().goTo("MainMenuScene"); // change the scene and go to the next one
    }
}

void IntroScene::render()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;

    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);
    float barWidth = 400;
    float barHeight = 20;
    float barX = GetScreenWidth() / 2 - barWidth / 2 - 50;
    float barY = GetScreenHeight() - 100;
    Color horrorRed = {170, 20, 20, 255};
    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, barY, barWidth * progress, barHeight, horrorRed);
    DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
}