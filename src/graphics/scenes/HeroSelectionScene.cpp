#include "graphics/scenes/HeroSelectionScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include "core/Game.hpp"

void HeroSelectionScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("HeroSelection", "assets/images/background/hero_selection.png");
    font = LoadFont("assets/fonts/simple.ttf");

    playerSelections[0] = -1;
    playerSelections[1] = -1;

    float size = 350.f;
    float spacing = 370.f;
    float totalWidth = HERO_COUNT * size + (HERO_COUNT - 1) * (spacing - size);
    float startX = (1600.0f - totalWidth) / 2.0f;
    float y = 250.f;

    for (int i = 0; i < HERO_COUNT; ++i)
        heroButtonRects[i] = {startX + i * spacing, y, size, size};

    LoadeHeroTextures();
    creatButton();
    whoStart();

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void HeroSelectionScene::onExit()
{
    UnloadFont(font);
    uiManager.clear();
}

void HeroSelectionScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    uiManager.update();

    Vector2 mousePos = GetMousePosition();

    if (phase == SelectionPhase::Player1_Select || phase == SelectionPhase::Player2_Select)
    {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            for (int i = 0; i < HERO_COUNT; ++i)
            {
                if (CheckCollisionPointRec(mousePos, heroButtonRects[i]) && isButtonEnabled(i))
                {
                    onHeroSelected(i);
                    break;
                }
            }
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            int &currentSelection = playerSelections[currentPlayer - 1];
            do
            {
                currentSelection = (currentSelection + 1) % HERO_COUNT;
            } while (!isButtonEnabled(currentSelection));
            onHeroSelected(currentSelection);
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            int &currentSelection = playerSelections[currentPlayer - 1];
            do
            {
                currentSelection = (currentSelection - 1 + HERO_COUNT) % HERO_COUNT;
            } while (!isButtonEnabled(currentSelection));
            onHeroSelected(currentSelection);
        }
    }
}

void HeroSelectionScene::creatButton()
{
    continueButton = std::make_unique<UIButton>(
        Rectangle{725, 700, 150, 50}, "SUBMIT", 30, WHITE, DARKGRAY, GRAY, WHITE);
    continueButton->setFont(font);
    continueButton->setFocus(true);
    continueButton->setOnClick([this]()
    {
        AudioManager::getInstance().playSoundEffect("click");

        if (playerSelections[currentPlayer - 1] == -1)
        {
            currentMessage = "Please select a hero before submitting!";
            return;
        }

        Game &game = Game::getInstance();

        if (playerSelections[0] == -1 || playerSelections[1] == -1)
        {
            if (playerSelections[0] == -1)
            {
                currentPlayer = 1;
                phase = SelectionPhase::Player1_Select;
                currentMessage = game.getName1() + ", choose your hero";
                return;
            }
            else if (playerSelections[1] == -1)
            {
                currentPlayer = 2;
                phase = SelectionPhase::Player2_Select;
                currentMessage = game.getName2() + ", choose your hero";
                return;
            }
        }

        try
        {
            game.reset();
            game.assignHeroes(
                game.getName1(),
                heroNameFromIndex(playerSelections[0]),
                heroNameFromIndex(playerSelections[1]));

            game.setupHeroes();
            game.setStartingPlater((game.getTime1() <= game.getTime2())
                ? game.getName1()
                : game.getName2());

            game.startNewGame();
        }
        catch (const GameException &ex)
        {
            std::cerr << "Error assigning heroes: " << ex.what() << std::endl;
            return;
        }

        currentMessage = "Both players have selected their heroes.";
        phase = SelectionPhase::Done;
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    });

    backButton = std::make_unique<UIButton>(
        Rectangle{40, 800, 200, 50}, "Main Menu", 30, WHITE, DARKGRAY, GRAY, WHITE);
    backButton->setFont(font);
    backButton->setOnClick([this]()
    {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE);
    });

    uiManager.add(std::move(continueButton));
    uiManager.add(std::move(backButton));
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
        int otherIndex = (currentPlayer == 1) ? playerSelections[1] : playerSelections[0];
        if (otherIndex != -1 && i == otherIndex)
            continue;

        Rectangle sourceRec = {0, 0, (float)heroTextures[i].width, (float)heroTextures[i].height};
        Rectangle destRect = heroButtonRects[i];
        Vector2 origin = {0, 0};

        DrawTexturePro(heroTextures[i], sourceRec, destRect, origin, 0.0f, WHITE);

        bool isSelected = (playerSelections[0] == i && currentPlayer == 1) ||
                          (playerSelections[1] == i && currentPlayer == 2);

        if (isSelected)
            DrawRectangleLinesEx(destRect, 5, WHITE);
        else if (CheckCollisionPointRec(mousePos, destRect) && isButtonEnabled(i))
            DrawRectangleLinesEx(destRect, 3, WHITE);
    }

    Vector2 textSize = MeasureTextEx(font, currentMessage.c_str(), 50, 0);
    float textX = (1600.0f - textSize.x) / 2.0f;
    DrawTextEx(font, currentMessage.c_str(), {textX, 50}, 50, 0, WHITE);

    uiManager.render();
    EndDrawing();
}


bool HeroSelectionScene::isButtonEnabled(int heroIndex) const
{
    if (phase != SelectionPhase::Player1_Select && phase != SelectionPhase::Player2_Select)
        return false;
    if (currentPlayer == 2 && heroIndex == playerSelections[0])
        return false;
    return true;
}

void HeroSelectionScene::onHeroSelected(int heroIndex)
{
    playerSelections[currentPlayer - 1] = heroIndex;
    std::cout << "[DEBUG] Player " << currentPlayer << " selected hero index " << heroIndex << "\n";
    currentMessage = "Click SUBMIT when ready!";
}

std::string HeroSelectionScene::heroNameFromIndex(int index) const
{
    switch (index)
    {
    case 0:
        return "Archaeologist";
    case 1:
        return "Courier";
    case 2:
        return "Mayor";
    case 3:
        return "Scientist";
    default:
        return "";
    }
}

void HeroSelectionScene::LoadeHeroTextures()
{
    heroTextures[0] = TextureManager::getInstance().getOrLoadTexture("Archaeologist", "assets/images/heroes/Archaeologist.png");
    heroTextures[1] = TextureManager::getInstance().getOrLoadTexture("Courier", "assets/images/heroes/Courier.png");
    heroTextures[2] = TextureManager::getInstance().getOrLoadTexture("Mayor", "assets/images/heroes/Mayor.png");
    heroTextures[3] = TextureManager::getInstance().getOrLoadTexture("Scientist", "assets/images/heroes/Scientist.png");

    for (int i = 0; i < HERO_COUNT; ++i)
        std::cout << "[DEBUG] Hero " << i << " tex size: "
                  << heroTextures[i].width << "x" << heroTextures[i].height << "\n";
}

void HeroSelectionScene::whoStart()
{
    Game &game = Game::getInstance();
    playerSelections[0] = -1;
    playerSelections[1] = -1;

    if (game.getTime1() <= game.getTime2())
    {
        currentPlayer = 1;
        phase = SelectionPhase::Player1_Select;
        currentMessage = game.getName1() + ", choose your hero";
    }
    else
    {
        currentPlayer = 2;
        phase = SelectionPhase::Player2_Select;
        currentMessage = game.getName2() + ", choose your hero";
    }
}
