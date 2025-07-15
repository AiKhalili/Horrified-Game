#include "graphics/scenes/IntroScene.hpp"
#include "graphics/scenes/SceneManager.hpp"

IntroScene::IntroScene()
{
    background = LoadTexture("assets/images/intro_bg.png");
    introMusic = LoadMusicStream("assets/audio/music/intro_theme.mp3");
    SetMusicVolume(introMusic, 0.7f);
}

IntroScene::~IntroScene()
{
    UnloadTexture(background);
    UnloadMusicStream(introMusic);
}

void IntroScene::update(float deleteTime)
{
    timer += deleteTime;

    if (!musicStarted)
    { // play for the first time
        PlayMusicStream(introMusic);
        musicStarted = true;
    }

    UpdateMusicStream(introMusic);

    if (progress < 1.0f)
    {
        progress += deleteTime * 0.2f;
    }
    if (timer >= 7.0f)
    {
        StopMusicStream(introMusic);
        SceneManager::getInstance().goTo("MainMenu"); // change the scene and go to the next one
    }
}

void IntroScene::render()
{
    ClearBackground(BLACK);
    DrawTexture(background, 0, 0, WHITE);
    float barWidth = 400;
    float barHeight = 20;
    float barX = GetScreenWidth() / 2 - barWidth / 2;
    barX -= 80;
    float barY = GetScreenHeight() - 130;
    Color horrorRed = {170, 20, 20, 255};
    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, barY, barWidth * progress, barHeight, horrorRed);
    DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
}