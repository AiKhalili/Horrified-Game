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
        SceneManager::getInstance().goTo("MainMenu"); // change the scene and go to the next one
    }
}

void IntroScene::render()
{
    ClearBackground(BLACK);
    DrawTexture(background, 0, 0, WHITE);
    float barWidth = 400;
    float barHeight = 20;
    float barX = GetScreenWidth() / 2 - barWidth / 2 - 80;
    float barY = GetScreenHeight() - 130;
    Color horrorRed = {170, 20, 20, 255};
    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, barY, barWidth * progress, barHeight, horrorRed);
    DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
}