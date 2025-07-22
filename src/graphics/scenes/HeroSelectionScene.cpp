#include "graphics/scenes/HeroSelectionScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include <iostream>

HeroSelectionScene::HeroSelectionScene()
{
    background = LoadTexture("assets/images/backhero.png");
    font = LoadFont("assets/fonts/simple.ttf");

    float startX = 50.f;
    float y = 250.f;
    float spacing = 370.f;
    float size = 350.f;

    for (int i = 0; i < HERO_COUNT; ++i)
    {
        heroButtonRects[i] = {startX + i * spacing, y, size, size};
    }

    heroTextures[0] = LoadTexture("assets/images/Heros/Archaeologist.png");
    heroTextures[1] = LoadTexture("assets/images/Heros/Courier.png");
    heroTextures[2] = LoadTexture("assets/images/Heros/Mayor.png");
    heroTextures[3] = LoadTexture("assets/images/Heros/Scientist.png");

    currentPlayer = 1;
}

HeroSelectionScene::~HeroSelectionScene()
{
    UnloadTexture(background);
    UnloadFont(font);
    for (int i = 0; i < HERO_COUNT; i++)
        UnloadTexture(heroTextures[i]);
}

void HeroSelectionScene::update(float deltaTime)
{
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        for (int i = 0; i < HERO_COUNT; ++i)
        {
            if (CheckCollisionPointRec(mousePos, heroButtonRects[i]))
            {
                if (isButtonEnabled(i))
                {
                    onHeroSelected(i);
                }
                else
                {
                    AudioManager::getInstance().playSoundEffect("click");
                }
                break;
            }
        }
    }
}

void HeroSelectionScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0}, 0.0f, WHITE);

    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < HERO_COUNT; ++i)
    {
        Rectangle sourceRec = {0, 0, (float)heroTextures[i].width, (float)heroTextures[i].height};
        Rectangle destRect = heroButtonRects[i];
        Vector2 origin = {0, 0};

        DrawTexturePro(heroTextures[i], sourceRec, destRect, origin, 0.0f, WHITE);

        // اگر موس روی عکس هست، کادر سفید رسم کن
        if (CheckCollisionPointRec(mousePos, destRect))
        {
            DrawRectangleLinesEx(destRect, 3, WHITE);
        }

        if (!isButtonEnabled(i))
        {
            DrawRectangleRec(destRect, Fade(BLACK, 0.6f));
        }
    }

    EndDrawing();
}

bool HeroSelectionScene::isButtonEnabled(int heroIndex) const
{
    return !(currentPlayer == 2 && heroIndex == playerSelections[0]);
}

void HeroSelectionScene::onHeroSelected(int heroIndex)
{
    playerSelections[currentPlayer - 1] = heroIndex;
    AudioManager::getInstance().playSoundEffect("click");

    if (currentPlayer == 1)
    {
        currentPlayer = 2;
    }
    else
    {
        std::cout << "Player 1 selected hero: " << playerSelections[0] << "\n";
        std::cout << "Player 2 selected hero: " << playerSelections[1] << "\n";
        SceneManager::getInstance().goTo("NextScene");
    }
}
