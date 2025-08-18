#include "graphics/scenes/HelpScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/Game.hpp"

void HelpScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("HelpActions", "assets/images/background/help_scene.png");
    font = LoadFont("assets/fonts/simple.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    createLabels();
    createButtons();
}

void HelpScene::onExit()
{
    UnloadFont(font);
    ui.clear();
}

void HelpScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void HelpScene::render()
{
    ClearBackground(BLACK);

    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0}, 0.0f, WHITE);

    ui.render();
}

void HelpScene::createLabels()
{
    const char *text = "Select which actions you want help with";
    int fontSize = 70;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 50}, text, fontSize, 0.0f, WHITE);
    selectText->setFont(font);
    ui.add(std::move(selectText));
}

void HelpScene::createButtons()
{
    float width = 140;
    float height = 50;
    float gapX = 20;
    float gapY = 15;

    int columns = 4;
    int rows = 2;

    float startX = 40;
    float startY = 900 - (rows * height + (rows - 1) * gapY) - 40;

    Color darkGreenGray = {70, 50, 35, 255};
    Color mediumGreenGray = {85, 65, 45, 255};
    Color textcolor = {245, 230, 196, 255};

    auto makeButton = [&](const std::string &text, int index, std::function<void()> action)
    {
        int row = index / columns;
        int col = index % columns;

        float x = startX + col * (width + gapX);
        float y = startY + row * (height + gapY);

        Rectangle bounds = {x, y, width, height};
        auto btn = std::make_unique<UIButton>(bounds, text, 24, textcolor, darkGreenGray, mediumGreenGray, textcolor);
        btn->setFont(font);
        btn->setOnClick([action]()
                        {
            AudioManager::getInstance().playSoundEffect("click");
            action(); });
        ui.add(std::move(btn));
    };

    makeButton("Move", 0, std::bind(&HelpScene::openMoveAction, this));
    makeButton("Pick Up", 1, std::bind(&HelpScene::openPickUpAction, this));
    makeButton("Defeat", 2, std::bind(&HelpScene::openDefeatAction, this));
    makeButton("Help", 3, std::bind(&HelpScene::openHelpAction, this));
    makeButton("Guide", 4, std::bind(&HelpScene::openGuideAction, this));
    makeButton("Advance", 5, std::bind(&HelpScene::openAdvanceAction, this));
    makeButton("Use Perk", 6, std::bind(&HelpScene::openUsePerkAction, this));
    makeButton("Special", 7, std::bind(&HelpScene::openSpecialAction, this));

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1450, 840, 120, 40}, "Main Menu", 20, darkGreenGray, textcolor, mediumGreenGray, darkGreenGray);
    menuBtn->setFont(font);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        Game::getInstance().reset();
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });
    ui.add(std::move(menuBtn));

    auto backBtn = std::make_unique<UIButton>(Rectangle{1450, 790, 120, 40}, "Back", 20, darkGreenGray, textcolor, mediumGreenGray, darkGreenGray);
    backBtn->setFont(font);
    backBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::SHOW_ACTIONS_SCENE); });
    ui.add(std::move(backBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1450, 740, 120, 40}, "Save", 20, darkGreenGray, textcolor, mediumGreenGray, darkGreenGray);
    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot("HelpScene");
        const std::string msg = "The game was successfully saved!";
                            showHelpMessage(msg);  });
    ui.add(std::move(saveBtn));
}

void HelpScene::openMoveAction()
{
    showHelpMessage("Move: Move to one of the neighboring locations");
}

void HelpScene::openPickUpAction()
{
    showHelpMessage("Pick Up: Pick up items from your current location");
}

void HelpScene::openDefeatAction()
{
    showHelpMessage("Defeat: Try to defeat a monster if its mission is complete and you're in the same location");
}

void HelpScene::openHelpAction()
{
    showHelpMessage("Help: Show this help menu");
}

void HelpScene::openGuideAction()
{
    showHelpMessage("Guide: Guide one or more villagers to a neighboring location");
}

void HelpScene::openAdvanceAction()
{
    showHelpMessage("Advance: Attempt to progress a monster's mission if you're in a valid location and have required items");
}

void HelpScene::openUsePerkAction()
{
    showHelpMessage("Use Perk: Use one of your Perk cards. This does not cost an action");
}

void HelpScene::openSpecialAction()
{
    showHelpMessage("Special Action: Use your hero's unique ability (if available)");
}

void HelpScene::showHelpMessage(const std::string &msg)
{
    if (msg.empty()) return;

    if (!helpLabel)
    {
        Color textcolor = {245, 230, 196, 255};
        auto tempLabel = std::make_unique<UILabel>(Vector2{800, 450}, msg, 42, 10.0f, textcolor, textcolor, true);
        tempLabel->setFont(font);
        tempLabel->enableBackground({70, 50, 35, 255}, 10.0f);
        helpLabel = tempLabel.get();
        ui.add(std::move(tempLabel));
    }
    else
    {
        helpLabel->setText(msg);
    }
}
