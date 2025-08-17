#include "graphics/scenes/MonstersSummaryScene.hpp"
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
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include "core/Game.hpp"

void MonstersSummaryScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture(
        "MonsterSummary", "assets/images/background/monster_summary.png");

    Titlefont = LoadFont("assets/fonts/spooky.otf");
    SetTextureFilter(Titlefont.texture, TEXTURE_FILTER_BILINEAR);

    Simplefont = LoadFont("assets/fonts/simple.ttf");
    SetTextureFilter(Simplefont.texture, TEXTURE_FILTER_BILINEAR);

    LoadMonsterAssetsTexturs();
    creatButtons();
    createLabels();

    AudioManager::getInstance().stopMusic();
    AudioManager::getInstance().playMonsterSelectMusic();
}

void MonstersSummaryScene::onExit()
{
    UnloadFont(Titlefont);
    UnloadFont(Simplefont);
    ui.clear();
    AudioManager::getInstance().stopMonsterSelectMusic();
    AudioManager::getInstance().playMusic();
}

void MonstersSummaryScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void MonstersSummaryScene::render()
{
    ClearBackground(BLACK);
    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0},
                   0.0f,
                   WHITE);

    float spacing = 50.0f;
    float imageWidth = 300.0f;
    float imageHeight = 420.0f;
    float circleSize = 450.0f;

    float totalWidth = (imageWidth * 3) + circleSize + (spacing * 3);
    float startX = (1600 - totalWidth) / 2.0f;
    float startY = (900 - imageHeight) / 2.0f;

    for (int i = 0; i < monsterassets.size(); i++)
    {
        int idx = i;

        if (i == monsterassets.size() - 1)
        {
            float cx = startX + i * (imageWidth + spacing);
            float cy = (900 - circleSize) / 2.0f;

            Rectangle src = {0, 0, (float)monsterassets[idx].width, (float)monsterassets[idx].height};
            Rectangle dest = {cx, cy, circleSize, circleSize};

            BeginScissorMode(dest.x, dest.y, dest.width, dest.height);
            DrawTexturePro(monsterassets[idx], src, dest, Vector2{0, 0}, 0.0f, WHITE);
            EndScissorMode();
        }
        else
        {
            Rectangle src = {0, 0, (float)monsterassets[idx].width, (float)monsterassets[idx].height};
            Rectangle dest = {startX + i * (imageWidth + spacing), startY, imageWidth, imageHeight};
            DrawTexturePro(monsterassets[idx], src, dest, Vector2{0, 0}, 0.0f, WHITE);
        }
    }

    ui.render();
}

void MonstersSummaryScene::LoadMonsterAssetsTexturs()
{
    monsterassets.resize(4);

    monsterassets[3] = TextureManager::getInstance().getOrLoadTexture("Precinct", "assets/images/Monster_Mat/Precinct.png");
    monsterassets[1] = TextureManager::getInstance().getOrLoadTexture("DraculaMap", "assets/images/Monster_Mat/DraculaMat.png");
    monsterassets[2] = TextureManager::getInstance().getOrLoadTexture("InvisibleManMap", "assets/images/Monster_Mat/InvisibleManMat.png");
    monsterassets[0] = TextureManager::getInstance().getOrLoadTexture("MonsterSetup", "assets/images/Monster_Mat/MonsterSetup.png");
}

void MonstersSummaryScene::createLabels()
{
    const char *text = "Monster Mat & Setup";
    int fontSize = 100;
    Vector2 textSize = MeasureTextEx(Titlefont, text, fontSize, 1);
    Color textcolor = WHITE;

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 30}, text, fontSize, 0.0f, textcolor);
    selectText->setFont(Titlefont);
    ui.add(std::move(selectText));

    auto tempLabel = std::make_unique<UILabel>(Vector2{600, 750}, "", 30, 3.0f, WHITE);
    tempLabel->setFont(Simplefont);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));
}

void MonstersSummaryScene::creatButtons()
{
    Color textcolor = {255, 255, 255, 150};
    Color labelcolor = {138, 3, 3, 180};
    Color clickcolor = {170, 30, 30, 150};

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1450, 840, 120, 40}, "Main Menu", 20, textcolor, labelcolor, clickcolor, textcolor);
    menuBtn->setFont(Simplefont);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        Game::getInstance().reset();
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });

    ui.add(std::move(menuBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1450, 790, 120, 40}, "Save", 20, textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(Simplefont);
    saveBtn->setOnClick([this]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot("MonstersSummaryScene");
        const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg); });

    ui.add(std::move(saveBtn));

    auto submtBtn = std::make_unique<UIButton>(Rectangle{690, 800, 150, 55}, "Continue", 45,
                                               labelcolor, textcolor, clickcolor, textcolor);
    submtBtn->setFont(Simplefont);
    submtBtn->setOnClick([this]()
                         { AudioManager::getInstance().playSoundEffect("click"); 
                        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });
    ui.add(std::move(submtBtn));
}

void MonstersSummaryScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}
