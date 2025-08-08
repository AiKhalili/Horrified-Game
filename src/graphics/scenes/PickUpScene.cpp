#include "graphics/scenes/PickUpScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include "graphics/scenes/ItemSelectionScene.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include "core/Game.hpp"

void PickUpScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("PickUpScene", "assets/images/background/pickup_scene.png");
    font = LoadFont("assets/fonts/simple.ttf");
    errorFont = LoadFont("assets/fonts/arial.ttf");

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(errorFont.texture, TEXTURE_FILTER_BILINEAR);

    hero = Game::getInstance().getCurrentHero();

    createLabels();
    createButtons();

    itemselect = SceneDataHub::getInstance().getSelectedItems();

    if (errorLabel)
    {
        errorLabel->setText("");
    }
}

void PickUpScene::onExit()
{
    UnloadFont(font);
    itemselect.clear();
    ui.clear();
}

void PickUpScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void PickUpScene::render()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);

    ui.render();
}

void PickUpScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}

void PickUpScene::createLabels()
{
    const char *text = "PickUp Scene";
    int fontSize = 75;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color textcolor = {255, 225, 130, 255};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 23}, text, fontSize, 0.0f, textcolor);
    selectText->setFont(font);
    ui.add(std::move(selectText));

    auto tempLabel = std::make_unique<UILabel>(Vector2{600, 800}, "", 30, 3.0f, WHITE);
    tempLabel->setFont(errorFont);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));
}

void PickUpScene::createButtons()
{
    float buttonWidth = 250.0f;
    float buttonHeight = 70.0f;
    float gapY = 50.0f;

    float centerX = GetScreenWidth() / 2.0f;
    float leftX = centerX - buttonWidth - 30.0f;
    float rightX = centerX + 30.0f;
    float topY = GetScreenHeight() / 2.0f - (buttonHeight * 1.5f + gapY) + 10.0f;

    int fontsize = 40;

    Color textcolor = {245, 233, 184, 255};
    Color labelcolor = {102, 76, 40, 220};
    Color clickcolor2 = {230, 220, 180, 255};
    Color clickcolor = {120, 95, 60, 200};

    auto saveBtn = std::make_unique<UIButton>(Rectangle{leftX, topY + gapY + buttonHeight, buttonWidth, buttonHeight}, "Save", fontsize,
                                              textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");
                            SaveManager::getInstance().saveGameToSlot("PickUpScene"); 
                            SceneDataHub::getInstance().reset(); 
                            const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg); });
    ui.add(std::move(saveBtn));

    auto boardBtn = std::make_unique<UIButton>(Rectangle{leftX, topY + 2 * (gapY + buttonHeight), buttonWidth, buttonHeight}, "Board Scene", fontsize,
                                               textcolor, labelcolor, clickcolor, textcolor);
    boardBtn->setFont(font);
    boardBtn->setOnClick([]()
                         {
                             AudioManager::getInstance().playSoundEffect("click");
                             SceneDataHub::getInstance().reset();
                             SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });
    ui.add(std::move(boardBtn));

    auto itemBtn = std::make_unique<UIButton>(Rectangle{centerX - buttonWidth / 2, topY, buttonWidth, buttonHeight}, "Item Select", fontsize,
                                              labelcolor, textcolor, clickcolor2, textcolor);
    itemBtn->setFont(font);
    itemBtn->setOnClick([this]()
                        {
                           AudioManager::getInstance().playSoundEffect("click");
                            auto& itemselect = SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE);
                            itemselect.setData(hero->getLocation()->get_items(), "PickUpScene");
                           SceneManager::getInstance().goTo(SceneKeys::ITEM_SELECTION_SCENE); });
    ui.add(std::move(itemBtn));

    auto submtBtn = std::make_unique<UIButton>(Rectangle{rightX, topY + gapY + buttonHeight, buttonWidth, buttonHeight}, "Back", fontsize,
                                               textcolor, labelcolor, clickcolor, textcolor);
    submtBtn->setFont(font);
    submtBtn->setOnClick([this]()
                         {
                             AudioManager::getInstance().playSoundEffect("click");
                             SceneDataHub::getInstance().reset();
                             SceneManager::getInstance().goTo(SceneKeys::SHOW_ACTIONS_SCENE); });
    ui.add(std::move(submtBtn));

    auto menuBtn = std::make_unique<UIButton>(Rectangle{rightX, topY + 2 * (gapY + buttonHeight), buttonWidth, buttonHeight}, "Main Menu", fontsize,
                                              textcolor, labelcolor, clickcolor, textcolor);
    menuBtn->setFont(font);
    menuBtn->setOnClick([]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");
                            SceneDataHub::getInstance().reset();
                            Game::getInstance().reset();
                            SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });
    ui.add(std::move(menuBtn));

    auto pickBtn = std::make_unique<UIButton>(Rectangle{centerX - buttonWidth / 2, topY + 3 * (gapY + buttonHeight), buttonWidth, buttonHeight},
                                              "Pick Up", fontsize, labelcolor, textcolor, clickcolor2, textcolor);
    pickBtn->setFont(font);
    pickBtn->setOnClick([this]()
                        {
    AudioManager::getInstance().playSoundEffect("click");

    try
    {
        if (itemselect.empty())
        {
            showErrorMessage("You cannot pick up if no items are selected.");
            return;
        }

        Game::getInstance().handlePickUP(itemselect);

        showErrorMessage("");
        SceneDataHub::getInstance().reset();
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    }
    catch (const GameException &ex)
    {
        showErrorMessage(ex.what());
    } });
    ui.add(std::move(pickBtn));
}
