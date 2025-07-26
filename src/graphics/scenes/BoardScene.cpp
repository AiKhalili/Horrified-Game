#include "graphics/scenes/BoardScene.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/TextureManager.hpp"
#include "core/Game.hpp"
#include "raymath.h"
#include "saves/SaveManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include <iostream>

void BoardScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("board", "assets/images/background/board.png");
    skullIcon = TextureManager::getInstance().getOrLoadTexture("skull", "assets/images/icon/skull.jpeg");
    chestIcon = TextureManager::getInstance().getOrLoadTexture("chest", "assets/images/icon/chest.png");
    cardIcon = TextureManager::getInstance().getOrLoadTexture("cardIcon", "assets/images/icon/perkCard.png");
    greenFlagIcon = TextureManager::getInstance().getOrLoadTexture("greenFlag", "assets/images/icon/greenFlag.png");

    normalFont = LoadFontEx("assets/fonts/simple.ttf", 100, 0, 0);
    SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);
    locationFont = LoadFont("assets/fonts/arial.ttf");

    makeButton("Action Select", 0, 0, std::bind(&BoardScene::handleSelectAction, this));
    makeButton("End Phase", 0, 1, std::bind(&BoardScene::handleExitGame, this));
    makeButton("Save & Exit", 1, 0, std::bind(&BoardScene::handleSaveAndExit, this));
    makeButton("Main Menu", 1, 1, std::bind(&BoardScene::handleGoToMainMenu, this));
    makeButton("Exit Game", 2, 0, std::bind(&BoardScene::handleExitGame, this), true);

    locations = {
        {{62, 225}, 40, "Cave"},
        {{195, 210}, 55, "Camp"},
        {{340, 180}, 60, "Precinct"},
        {{480, 153}, 55, "Inn"},
        {{645, 175}, 55, "Barn"},
        {{788, 182}, 50, "Dungeon"},
        {{716, 328}, 55, "Tower"},
        {{545, 340}, 60, "Theatre"},
        {{768, 474}, 57, "Docks"},
        {{240, 465}, 60, "Mansion"},
        {{87, 530}, 55, "Abbey"},
        {{62, 682}, 50, "Crypt"},
        {{190, 667}, 60, "Museum"},
        {{324, 808}, 47, "Hospital"},
        {{387, 698}, 60, "Church"},
        {{508, 795}, 60, "Graveyard"},
        {{690, 790}, 60, "Institute"},
        {{590, 666}, 60, "Laboratory"},
        {{473, 563}, 54, "Shop"}};
}

void BoardScene::makeButton(const std::string &text, int row, int col, std::function<void()> onClick, bool center)
{
    float buttonWidth = 220;
    float buttonHeight = 60;
    float gapX = 40;
    float gapY = 30;
    float startX = 1000;
    float startY = 600;

    Color bgColor = {0, 51, 102, 255};
    Color fontColor = {255, 230, 180, 255};

    float x;
    if (center)
    {
        float totalWidth = (buttonWidth * 2 + gapX);
        x = startX + (totalWidth - buttonWidth) / 2.0f;
    }
    else
    {
        x = startX + col * (buttonWidth + gapX);
    }

    Rectangle bounds = {
        x,
        startY + row * (buttonHeight + gapY),
        buttonWidth,
        buttonHeight};

    auto btn = std::make_unique<UIButton>(bounds, text, 32, fontColor, bgColor, DARKGRAY, fontColor);
    btn->setFont(normalFont);
    btn->setOnClick([onClick]()
                    {
        AudioManager::getInstance().playSoundEffect("click");
        onClick(); });
    ui.add(std::move(btn));
}

void BoardScene::handleSelectAction()
{
    SceneManager::getInstance().goTo("SelectActionScene");
}

void BoardScene::handleSaveAndExit()
{
    SaveManager::getInstance().saveGameToSlot();
    CloseWindow();
}

void BoardScene::handleGoToMainMenu()
{
    SceneManager::getInstance().goTo("MainMenuScene");
}

void BoardScene::handleExitGame()
{
    CloseWindow();
}

void BoardScene::onExit()
{
    UnloadFont(normalFont);
    ui.clear();
}

void BoardScene::handleClickOnLocation()
{
    if (!hoveredLocation.empty() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        AudioManager::getInstance().playSoundEffect("click");
        std::cout << "Clicked on : " << hoveredLocation << '\n'; // just for debug
        SceneManager::getInstance().goTo("LocationInfoScene");
    }
}

void BoardScene::checkLocationHover()
{ // is the mouse pointer hovering over one of the locations or not?
    hoveredLocation.clear();
    Vector2 mousePos = GetMousePosition();

    for (const auto &loc : locations)
    {
        float dist = Vector2Distance(mousePos, loc.posision);
        if (dist < loc.radius)
        {
            hoveredLocation = loc.locatonName;
            break;
        }
    }
}

void BoardScene::DrawTerrorLevel()
{
    int level = Game::getInstance().getTerrorLevel();

    Vector2 start = {1885, 155};

    float areaWidth = 1160.0f;
    float areaHeight = 100.0f;
    float originalSize = 736.0f;
    float maxWidthPerIcon = areaWidth / 5.0f;
    float maxHeightPerIcon = areaHeight;

    float scaleX = maxWidthPerIcon / originalSize;
    float scaleY = maxHeightPerIcon / originalSize;
    float scale = std::min(scaleX, scaleY);
    float iconSize = originalSize * scale;
    float gap = iconSize;

    for (int i = 0; i < level && i < 5; ++i)
    {
        float x = start.x + i * gap;
        float y = start.y + (areaHeight - iconSize) / 2.0f;
        DrawTextureEx(skullIcon, {x, y}, 0, scale, WHITE);
    }
}

void BoardScene::drawHeroInfo()
{
    Hero *hero = Game::getInstance().getCurrentHero();
    if (!hero)
    {
        std::cerr << "No hero found for draw hero info!\n";
    }

    std::string heroName = hero->getClassName();
    std::string heroPath = "assets/images/heroes/" + heroName + ".png";

    Texture2D heroTexture = TextureManager::getInstance().getOrLoadTexture(heroName, heroPath);

    Rectangle destHero = {1200, 70, 400, 500};

    DrawTexturePro(heroTexture,
                   {0, 0, (float)heroTexture.width, (float)heroTexture.height},
                   destHero, {0, 0}, 0.0f, WHITE);

    Rectangle destChest = {950, 220, 200, 130};
    Rectangle destCard = {950, 375, 200, 130};

    Vector2 mousePos = GetMousePosition();

    bool hoveringChest = CheckCollisionPointRec(mousePos, destChest);
    bool hoveringCard = CheckCollisionPointRec(mousePos, destCard);

    if (hoveringChest)
    {
        DrawRectangleLinesEx(destChest, 4.0f, Color{255, 230, 150, 180});
    }
    if (hoveringCard)
    {
        DrawRectangleLinesEx(destCard, 4.0f, Color{255, 230, 150, 180});
    }

    DrawTexturePro(chestIcon,
                   {0, 0, (float)chestIcon.width, (float)chestIcon.height},
                   destChest, {0, 0}, 0.0f, WHITE);

    const char *chestLabel = "Your Items";
    Vector2 chestSize = MeasureTextEx(normalFont, chestLabel, 28, 1);
    Vector2 chestTextPos = {
        destChest.x + destChest.width / 2 - chestSize.x / 2,
        destChest.y + destChest.height};
    DrawTextEx(normalFont, chestLabel, chestTextPos, 28, 1, {0, 0, 100, 255});

    DrawTexturePro(cardIcon,
                   {0, 0, (float)cardIcon.width, (float)cardIcon.height},
                   destCard, {0, 0}, 0.0f, WHITE);

    const char *cardLabel = "Your Perk Cards";
    Vector2 cardSize = MeasureTextEx(normalFont, cardLabel, 28, 1);
    Vector2 cardTextPos = {
        destCard.x + destCard.width / 2 - cardSize.x / 2,
        destCard.y + destCard.height + 8};
    DrawTextEx(normalFont, cardLabel, cardTextPos, 28, 1, {0, 0, 100, 255});
}

void BoardScene::update(float)
{
    checkLocationHover();
    handleClickOnLocation();
    handleHeroInfoClick();

    Game::getInstance().update();

    handleGameStateTransition();

    AudioManager::getInstance().update();
    ui.update();
}

void BoardScene::handleGameStateTransition()
{
    Game &game = Game::getInstance();
    GameState state = game.getGameState();

    switch (state)
    {
    case GameState::GameWon:
        SceneManager::getInstance().goTo(SceneKeys::GAME_WON_SCENE);
        break;
    case GameState::GameLost:
        SceneManager::getInstance().goTo(SceneKeys::GAME_LOST_SCENE);
        break;
    case GameState::GameOutOfTime:
        SceneManager::getInstance().goTo(SceneKeys::OUT_OF_TIME_SCENE);
        break;
    case GameState::ShowPerkCard:
        SceneManager::getInstance().goTo(SceneKeys::PERK_CARD_SCENE);
        break;
    case GameState::StartMonsterPhase:
        SceneManager::getInstance().goTo(SceneKeys::MONSTER_PHASE_SCENE);
        break;
    default:
        break;
    }
}

void BoardScene::drawGlow()
{
    if (!hoveredLocation.empty())
    {
        for (const auto &loc : locations)
        {
            if (loc.locatonName == hoveredLocation)
            {
                Color neonGreen = {57, 255, 20, 255};
                for (int i = 0; i < 20; i++)
                {
                    float radius = loc.radius + i;
                    float alpha = 1.0f - (float)i / 20;
                    Color ringColor = Color{neonGreen.r, neonGreen.g, neonGreen.b, (unsigned char)(alpha * 255)};
                    DrawCircleLinesV(loc.posision, radius, ringColor);
                }
                break;
            }
        }
    }
}

void BoardScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    float scale = 0.5f;
    Vector2 position = {
        (1600 - background.width * scale) / 2.0f,
        (900 - background.height * scale) / 2.0f};

    DrawTextureEx(background, position, 0.0f, scale, WHITE);
    drawGlow();

    drawHeroPositionMarker();
    DrawTerrorLevel();
    drawHeroInfo();
    drawHeroActions();

    ui.render();

    if (!hoveredLocation.empty())
    {
        Vector2 pos = {(float)GetMouseX() + 15, (float)GetMouseY() - 20};
        DrawTextEx(locationFont, hoveredLocation.c_str(), pos, 28, 1, {0, 0, 0, 255});
    }

    EndDrawing();
}

void BoardScene::drawHeroPositionMarker()
{
    Hero *hero = Game::getInstance().getCurrentHero();
    if (!hero)
    {
        std::cerr << "No hero found for draw hero position marker!\n";
    }

    std::string heroLocation = hero->getLocation()->get_name();

    for (const auto &loc : locations)
    {
        if (loc.locatonName == heroLocation)
        {
            float originalSize = (float)greenFlagIcon.width;
            float desiredSize = loc.radius * 2.3f;
            float scale = desiredSize / originalSize;
            float x = loc.posision.x - (greenFlagIcon.width * scale) / 2;
            float y = loc.posision.y - (greenFlagIcon.height * scale);

            DrawTextureEx(greenFlagIcon, {x, y}, 0.0f, scale, WHITE);
            break;
        }
    }
}

void BoardScene::handleHeroInfoClick()
{
    Vector2 mousePos = GetMousePosition();

    Rectangle destChest = {950, 230, 200, 130};
    Rectangle destCard = {950, 375, 200, 130};

    if (CheckCollisionPointRec(mousePos, destChest) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo("ChestInfoScene");
    }

    if (CheckCollisionPointRec(mousePos, destCard) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo("PerkCardScene");
    }
}

void BoardScene::drawHeroActions()
{
    Hero *hero = Game::getInstance().getCurrentHero();
    if (!hero)
    {
        std::cerr << "Hero not valid to draw action left in board game scene!\n";
        return;
    }

    int maxAction = hero->getMaxActions();
    int remaining = hero->getActionsLeft();

    float startX = 950;
    float startY = 150;
    float radius = 18.0f;
    float gap = 12.0f;

    for (int i = 0; i < maxAction; i++)
    {
        Color color = (i < remaining) ? Color{255, 250, 100, 255} : DARKGRAY;
        Vector2 pos = {startX + i * (radius * 2 + gap), startY};
        DrawCircleV(pos, radius, color);
        DrawCircleLinesV(pos, radius, BLACK);
    }
}