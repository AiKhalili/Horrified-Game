#include "graphics/scenes/PlayerSummaryScene.hpp"
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

void PlayerSummaryScene::setData(const std::string &player1Name, const std::string &hero1,
                                 const std::string &player2Name, const std::string &hero2)
{
    p1Name = player1Name;
    h1Name = hero1;
    p2Name = player2Name;
    h2Name = hero2;
}

void PlayerSummaryScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture(
        "PlayerSummary", "assets/images/background/player_summary.png");

    Titlefont = LoadFont("assets/fonts/spooky.otf");
    SetTextureFilter(Titlefont.texture, TEXTURE_FILTER_BILINEAR);

    Simplefont = LoadFont("assets/fonts/simple.ttf");
    SetTextureFilter(Simplefont.texture, TEXTURE_FILTER_BILINEAR);

    LoadHeroTexturs();
    creatButtons();
    createLabels();
}

void PlayerSummaryScene::onExit()
{
    UnloadFont(Titlefont);
    UnloadFont(Simplefont);
    ui.clear();
}

void PlayerSummaryScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void PlayerSummaryScene::render()
{
    ClearBackground(BLACK);
    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0},
                   0.0f,
                   WHITE);

    // Draw player 1 hero
    DrawTexturePro(hero1Texture,
                   Rectangle{0, 0, (float)hero1Texture.width, (float)hero1Texture.height},
                   Rectangle{240, 300, 450, 450},
                   Vector2{0, 0},
                   0.0f,
                   WHITE);

    // Draw player 2 hero
    DrawTexturePro(hero2Texture,
                   Rectangle{0, 0, (float)hero2Texture.width, (float)hero2Texture.height},
                   Rectangle{850, 300, 450, 450},
                   Vector2{0, 0},
                   0.0f,
                   WHITE);

    ui.render();
}

void PlayerSummaryScene::LoadHeroTexturs()
{
    hero1Texture = TextureManager::getInstance().getOrLoadTexture(h1Name, "assets/images/heroes/" + h1Name + ".png");
    hero2Texture = TextureManager::getInstance().getOrLoadTexture(h2Name, "assets/images/heroes/" + h2Name + ".png");
}

void PlayerSummaryScene::createLabels()
{
    const char *text = "The war begins";
    int fontSize = 80;
    Vector2 textSize = MeasureTextEx(Titlefont, text, fontSize, 1);
    Color textcolor = {245, 230, 196, 255};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 45}, text, fontSize, 0.0f, textcolor);
    selectText->setFont(Titlefont);
    ui.add(std::move(selectText));

    Color labelcolor = {40, 25, 10, 255};
    Color texttcolor = {243, 226, 177, 255};

    int nameFontSize = 50;

    auto select1Text = std::make_unique<UILabel>(
        Vector2{470, 230}, p1Name, nameFontSize, 0.0f, texttcolor, texttcolor, true);
    select1Text->setFont(Simplefont);
    select1Text->enableBackground(labelcolor, 10.0f);
    ui.add(std::move(select1Text));

    auto select2Text = std::make_unique<UILabel>(
        Vector2{1065, 230}, p2Name, nameFontSize, 0.0f, texttcolor, texttcolor, true);
    select2Text->setFont(Simplefont);
    select2Text->enableBackground(labelcolor, 10.0f);
    ui.add(std::move(select2Text));

    auto tempLabel = std::make_unique<UILabel>(Vector2{800, 760}, "", 30, 3.0f, WHITE, WHITE, true);
    tempLabel->setFont(Simplefont);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));
}

void PlayerSummaryScene::creatButtons()
{
    Color textcolor = {245, 230, 196, 255};
    Color labelcolor = {70, 50, 35, 255};
    Color clickcolor = {85, 65, 45, 255};

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1450, 840, 120, 40}, "Main Menu", 20, textcolor, labelcolor, clickcolor, textcolor);
    menuBtn->setFont(Simplefont);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });

    ui.add(std::move(menuBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1450, 790, 120, 40}, "Save", 20, textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(Simplefont);
    saveBtn->setOnClick([this]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot("PlayerSummaryScene");
        const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg); });

    ui.add(std::move(saveBtn));

    auto submtBtn = std::make_unique<UIButton>(Rectangle{690, 800, 150, 55}, "Continue", 45,
                                               labelcolor, textcolor, clickcolor, textcolor);
    submtBtn->setFont(Simplefont);
    submtBtn->setOnClick([this]()
                         { AudioManager::getInstance().playSoundEffect("click"); 
                        SceneManager::getInstance().goTo(SceneKeys::MONSTERS_SUMMARY_SCENE); });
    ui.add(std::move(submtBtn));
}

void PlayerSummaryScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}

std::vector<std::string> PlayerSummaryScene::getData()
{
    std::vector<std::string> data;
    data.resize(4);
    data[0] = p1Name;
    data[1] = h1Name;
    data[2] = p2Name;
    data[3] = h2Name;

    return data;
}