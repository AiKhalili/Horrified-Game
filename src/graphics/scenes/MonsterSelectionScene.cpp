#include "graphics/scenes/MonsterSelectionScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include "core/SceneDataHub.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>
#include "core/Game.hpp"

void MonsterSelectionScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("MonsterSelection", "assets/images/background/monster_selection.png");
    font = LoadFont("assets/fonts/simple.ttf");
    LoadeMonsterTextures();

    createButtons();
    createLabels();

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    AudioManager::getInstance().stopMusic();
    AudioManager::getInstance().playMonsterSelectMusic();
}

void MonsterSelectionScene::onExit()
{
    UnloadFont(font);
    ui.clear();
    AudioManager::getInstance().stopMonsterSelectMusic();
    AudioManager::getInstance().playMusic();
}

void MonsterSelectionScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    ui.update();

    Vector2 mousePos = GetMousePosition();
    float scale = 0.5f;

    Rectangle leftRect = {200, (GetScreenHeight() / 2.0f) - (monsterTextures[0].height * scale / 2),
                          monsterTextures[0].width * scale, monsterTextures[0].height * scale};
    Rectangle rightRect = {GetScreenWidth() - 200 - monsterTextures[1].width * scale,
                           (GetScreenHeight() / 2.0f) - (monsterTextures[1].height * scale / 2),
                           monsterTextures[1].width * scale, monsterTextures[1].height * scale};

    if (CheckCollisionPointRec(mousePos, leftRect))
        hoverMonster = 0;
    else if (CheckCollisionPointRec(mousePos, rightRect))
        hoverMonster = 1;
    else
        hoverMonster = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoverMonster != -1)
    {
        if (clickedMonster == hoverMonster)
        {
            clickedMonster = -1;
        }
        else
        {
            clickedMonster = hoverMonster;
        }
    }
}

void MonsterSelectionScene::render()
{
    float baseScale = 0.3f;
    float hoverScale = 0.5f;
    float gap = 100;
    float centerX = GetScreenWidth() / 2.0f;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    Vector2 bgPos = {0, 0};
    float scaleX = (float)GetScreenWidth() / background.width;
    float scaleY = (float)GetScreenHeight() / background.height;
    float scale = fmax(scaleX, scaleY);
    DrawTextureEx(background, bgPos, 0.0f, scale, WHITE);

    Color haloColor = ColorAlpha(WHITE, 0.3f);

    float leftScale = baseScale;
    float rightScale = baseScale;

    if (hoverMonster == 0 && clickedMonster != 0)
        leftScale = hoverScale;
    if (hoverMonster == 1 && clickedMonster != 1)
        rightScale = hoverScale;

    if (clickedMonster == 0)
        leftScale = hoverScale;
    if (clickedMonster == 1)
        rightScale = hoverScale;

    Vector2 leftPos = {
        centerX - gap - monsterTextures[0].width * leftScale,
        (GetScreenHeight() / 2.0f) - (monsterTextures[0].height * leftScale / 2)};
    DrawCircle(leftPos.x + (monsterTextures[0].width * leftScale / 2),
               leftPos.y + (monsterTextures[0].height * leftScale / 2),
               (monsterTextures[0].width * leftScale / 2) + 20,
               haloColor);
    DrawTextureEx(monsterTextures[0], leftPos, 0.0f, leftScale, WHITE);

    Vector2 rightPos = {
        centerX + gap,
        (GetScreenHeight() / 2.0f) - (monsterTextures[1].height * rightScale / 2)};
    DrawCircle(rightPos.x + (monsterTextures[1].width * rightScale / 2),
               rightPos.y + (monsterTextures[1].height * rightScale / 2),
               (monsterTextures[1].width * rightScale / 2) + 20,
               haloColor);
    DrawTextureEx(monsterTextures[1], rightPos, 0.0f, rightScale, WHITE);

    ui.render();

    EndDrawing();
}

void MonsterSelectionScene::LoadeMonsterTextures()
{
    monsterTextures[0] = TextureManager::getInstance().getOrLoadTexture("Dracula", "assets/images/Monsters/Dracula.png");
    monsterTextures[1] = TextureManager::getInstance().getOrLoadTexture("InvisibleMan", "assets/images/Monsters/InvisibleMan.png");
}

void MonsterSelectionScene::createButtons()
{
    Color textcolor = {255, 255, 255, 150};
    Color labelcolor = {138, 3, 3, 180};
    Color clickcolor = {170, 30, 30, 150};

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1450, 840, 120, 40}, "Main Menu", 20, textcolor, labelcolor, clickcolor, textcolor);
    menuBtn->setFont(font);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });
    ui.add(std::move(menuBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1450, 790, 120, 40}, "Save", 20, textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(font);
    saveBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot(); });
    ui.add(std::move(saveBtn));

    auto boardBtn = std::make_unique<UIButton>(Rectangle{1450, 740, 120, 40}, "Board Scene", 20, textcolor, labelcolor, clickcolor, textcolor);
    boardBtn->setFont(font);
    boardBtn->setOnClick([]()
                         {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });
    ui.add(std::move(boardBtn));

    auto submtBtn = std::make_unique<UIButton>(Rectangle{745, 590, 120, 60}, "Submit", 40,
                                               textcolor, labelcolor, clickcolor, textcolor);
    submtBtn->setFont(font);
    submtBtn->setOnClick([this]()
                         {
    AudioManager::getInstance().playSoundEffect("click");

    confirmedMonster = clickedMonster;

    if (confirmedMonster == 0)
        selectedMonster = new Dracula();
    else if (confirmedMonster == 1)
        selectedMonster = new Invisible_Man();
    else
        selectedMonster = nullptr; 
    SceneDataHub::getInstance().setSelectedMonster(this->getSelectedMonster()); });

    ui.add(std::move(submtBtn));
}

Monster *MonsterSelectionScene::getSelectedMonster()
{
    return selectedMonster;
}

void MonsterSelectionScene::createLabels()
{
     const char *text = "Please Select Monster";
    int fontSize = 45;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color textcolor = {255, 255, 255, 150};
    Color labelcolor = {138, 3, 3, 180};
    Color clickcolor = {170, 30, 30, 150};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 15}, text, fontSize, 0.0f, textcolor);
    selectText->setFont(font);
    ui.add(std::move(selectText));

    auto nameBtn = std::make_unique<UILabel>(Vector2{390, 700}, "Monster Name:  Dracula", 30, 0.0f,
                                              labelcolor, labelcolor);
    nameBtn->setFont(font);
    nameBtn->enableBackground(textcolor, 10.0f);
    ui.add(std::move(nameBtn));

    auto wealthBtn = std::make_unique<UILabel>(Vector2{440, 765}, "Wealth: Coffins", 30,0.0f,
                                                labelcolor, labelcolor);
    wealthBtn->setFont(font);
    wealthBtn->enableBackground(textcolor, 10.0f);
    ui.add(std::move(wealthBtn));

    auto name1Btn = std::make_unique<UILabel>(Vector2{890, 700}, "Monster Name:  InvisibleMan", 30,0.0f,
                                               labelcolor, labelcolor);
    name1Btn->setFont(font);
    name1Btn->enableBackground(textcolor, 10.0f);
    ui.add(std::move(name1Btn));

    auto wealth1Btn = std::make_unique<UILabel>(Vector2{960, 765}, "Wealth: Evidences", 30,0.0f,
                                                 labelcolor, labelcolor);
    wealth1Btn->setFont(font);
    wealth1Btn->enableBackground(textcolor, 10.0f);
    ui.add(std::move(wealth1Btn));
}