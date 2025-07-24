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
    cardIcon = TextureManager::getInstance().getOrLoadTexture("cardIcon", "assets/images/icon/perkCard.jpg");
    cardIcon = TextureManager::getInstance().getOrLoadTexture("greenFlag", "assets/images/icon/greenFlag.png");

    normalFont = LoadFont("assets/fonts/simple.ttf");

    makeButton("Action Select", 0, 0, std::bind(&BoardScene::handleSelectAction, this));
    makeButton("Save & Exit", 0, 1, std::bind(&BoardScene::handleSaveAndExit, this));
    makeButton("Main Menu", 1, 0, std::bind(&BoardScene::handleGoToMainMenu, this));
    makeButton("Exit Game", 1, 1, std::bind(&BoardScene::handleExitGame, this));

    locations = {
        {{124, 450}, 85, "Cave"},
        {{388, 421}, 110, "Camp"},
        {{678, 365}, 110, "Precinct"},
        {{960, 310}, 115, "Inn"},
        {{1295, 355}, 100, "Barn"},
        {{1574, 351}, 100, "Dungeon"},
        {{1430, 645}, 120, "Tower"},
        {{1086, 681}, 115, "Theatre"},
        {{1535, 947}, 110, "Docks"},
        {{476, 943}, 115, "Mansion"},
        {{174, 1060}, 100, "Abbey"},
        {{120, 1367}, 105, "Crypt"},
        {{380, 1337}, 120, "Museum"},
        {{648, 1613}, 100, "Hospital"},
        {{772, 1393}, 115, "Church"},
        {{1022, 1585}, 120, "Graveyard"},
        {{1385, 1595}, 100, "Institute"},
        {{1180, 1327}, 130, "Laboratory"},
        {{942, 1137}, 105, "Shop"}};
}

void BoardScene::makeButton(const std::string &text, int row, int col, std::function<void()> onClick)
{
    float buttonWidth = 200;
    float buttonHeight = 60;
    float gapX = 40;
    float gapY = 30;
    float startX = 1900;
    float startY = 1100;

    Color bgColor = {0, 51, 102, 255};
    Color fontColor = {255, 230, 180, 255};

    Rectangle bounds = {
        startX + col * gapX,
        startY + row * gapY,
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
    std::string heroPath = "assets/images/Heroes/" + heroName + ".png";

    Texture2D heroTexture = TextureManager::getInstance().getOrLoadTexture(heroName, heroPath);

    Rectangle destHero = {2450, 215, 700, 580};

    DrawTexturePro(heroTexture,
                   {0, 0, (float)heroTexture.width, (float)heroTexture.height},
                   destHero, {0, 0}, 0.0f, WHITE);

    Rectangle destChest = {2000, 400, 300, 180};
    Rectangle destCard = {2000, 650, 300, 180};

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

    DrawTexturePro(cardIcon,
                   {0, 0, (float)cardIcon.width, (float)cardIcon.height},
                   destCard, {0, 0}, 0.0f, WHITE);
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

void BoardScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexture(background, 0, 0, WHITE);

    if (!hoveredLocation.empty())
    {
        for (const auto &loc : locations)
        {
            if (loc.locatonName == hoveredLocation)
            {
                for (int i = 0; i < 3; i++)
                {
                    float radius = loc.radius + i * 5;
                    int alpha = 100 - i * 30;
                    Color neonGreen = {57, 255, 20, (unsigned char)alpha};
                    DrawCircleLines(loc.posision.x, loc.posision.y, radius, neonGreen);
                }
                break;
            }
        }
    }

    drawHeroPositionMarker();
    DrawTerrorLevel();
    drawHeroInfo();

    ui.render();

    if (!hoveredLocation.empty())
    {
        DrawText(hoveredLocation.c_str(), GetMouseX() + 15, GetMouseY() - 20, 28, RAYWHITE);
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
            float desiredSize = loc.radius * 1.2f;
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

    Rectangle destChest = {2000, 400, 300, 180};
    Rectangle destCard = {2000, 650, 300, 180};

    if (CheckCollisionPointRec(mousePos, destChest) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo("ChetInfoScene");
    }

    if (CheckCollisionPointRec(mousePos, destCard) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo("PerkCardScene");
    }
}