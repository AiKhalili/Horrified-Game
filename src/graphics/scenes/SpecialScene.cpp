#include "graphics/scenes/SpecialScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include "graphics/scenes/ItemSelectionScene.hpp"
#include "graphics/scenes/LocationSelectionScene.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include "core/Game.hpp"

void SpecialScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("SpecialScene", "assets/images/background/special_scene.png");
    font = LoadFont("assets/fonts/simple.ttf");
    errorFont = LoadFont("assets/fonts/arial.ttf");

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(errorFont.texture, TEXTURE_FILTER_BILINEAR);

    hero = Game::getInstance().getCurrentHero();

    createLabels();
    createButtons();

    locselect = SceneDataHub::getInstance().getSelectedLocation();
    itemselect = SceneDataHub::getInstance().getSelectedItems();

    if (errorLabel)
    {
        errorLabel->setText("");
    }
}

void SpecialScene::onExit()
{
    UnloadFont(font);
    itemselect.clear();
    locselect = nullptr;
    ui.clear();
}

void SpecialScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void SpecialScene::render()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);

    ui.render();
}

void SpecialScene::createLabels()
{
    const char *text = "Special Scene";
    int fontSize = 75;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color textcolor = {255, 225, 130, 255};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 15}, text, fontSize, 0.0f, textcolor);
    selectText->setFont(font);
    ui.add(std::move(selectText));

    auto tempLabel = std::make_unique<UILabel>(Vector2{600, 800}, "", 30, 3.0f, WHITE);
    tempLabel->setFont(errorFont);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));
}

void SpecialScene::createButtons()
{
    float buttonWidth = 250.0f;
    float buttonHeight = 70.0f;
    float gapY = 50.0f;

    float centerX = GetScreenWidth() / 2.0f;
    float leftX = centerX - buttonWidth - 30.0f;
    float rightX = centerX + 30.0f;
    float topY = GetScreenHeight() / 2.0f - (buttonHeight * 1.5f + gapY) + 40.0f;

    int fontsize = 40;

    Color textcolor = {255, 255, 200, 255};
    Color labelcolor = {20, 40, 50, 255};
    Color clickcolor = {40, 70, 90, 180};
    Color clickcolor2 = {230, 230, 180, 180};

    if (auto *arch = dynamic_cast<Archaeologist *>(hero))
    {
        auto itemBtn = std::make_unique<UIButton>(Rectangle{rightX, topY, buttonWidth, buttonHeight}, "Item Select", fontsize,
                                                  labelcolor, textcolor, clickcolor2, textcolor);
        itemBtn->setFont(font);
        itemBtn->setOnClick([this]()
                            {
                            AudioManager::getInstance().playSoundEffect("click");

                            if(locselect == nullptr)
                            {
                                showErrorMessage("Please first select a location!");
                                return;
                            }

                            auto& itemselect = SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE);
                            itemselect.setData(locselect->get_items(), "SpecialScene");
                            SceneManager::getInstance().goTo(SceneKeys::ITEM_SELECTION_SCENE); });
        ui.add(std::move(itemBtn));

        auto locBtn = std::make_unique<UIButton>(Rectangle{leftX, topY, buttonWidth, buttonHeight}, "Location Select", fontsize,
                                                 labelcolor, textcolor, clickcolor2, textcolor);
        locBtn->setFont(font);
        locBtn->setOnClick([this]()
                           {
                           AudioManager::getInstance().playSoundEffect("click");
                            auto& locationselect = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
                            locationselect.setData(hero->getLocation()->get_neighbors(), "SpecialScene");
                           SceneManager::getInstance().goTo(SceneKeys::LOCATION_SELECTION_SCENE); });
        ui.add(std::move(locBtn));

        auto moveBtn = std::make_unique<UIButton>(Rectangle{centerX - buttonWidth / 2, topY + 3 * (gapY + buttonHeight), buttonWidth, buttonHeight},
                                                  "Special", fontsize, labelcolor, textcolor, clickcolor2, textcolor);
        moveBtn->setFont(font);
        moveBtn->setOnClick([this]()
                            {

        AudioManager::getInstance().playSoundEffect("click");
    try
    {
        if (locselect == nullptr || itemselect.empty())
        {
            showErrorMessage("Please select a location and items!");
            return;
        }

       Game::getInstance().handleSpecialAction(locselect, itemselect);

        showErrorMessage("");
        SceneDataHub::getInstance().reset();
        AudioManager::getInstance().playSoundEffect("magic");
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    }
    catch (const GameException &ex)
    {
        AudioManager::getInstance().playSoundEffect("click");
        showErrorMessage(ex.what());
    } });
        ui.add(std::move(moveBtn));
    }
    else if (auto *courier = dynamic_cast<Courier *>(hero))
    {
        auto moveBtn = std::make_unique<UIButton>(Rectangle{centerX - buttonWidth / 2, topY + 3 * (gapY + buttonHeight), buttonWidth, buttonHeight},
                                                  "Special", fontsize, labelcolor, textcolor, clickcolor2, textcolor);
        moveBtn->setFont(font);
        moveBtn->setOnClick([this]()
                            {
    AudioManager::getInstance().playSoundEffect("magic");

    try
    {
        Game::getInstance().handleSpecialAction(nullptr, {});

        SceneDataHub::getInstance().reset();
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    }
    catch (const GameException &ex)
    {
        showErrorMessage(ex.what());
    } });
        ui.add(std::move(moveBtn));
    }

    auto saveBtn = std::make_unique<UIButton>(Rectangle{leftX, topY + gapY + buttonHeight, buttonWidth, buttonHeight}, "Save", fontsize,
                                              textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");
                            SaveManager::getInstance().saveGameToSlot("SpecialScene");
                            const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg);  });
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
                            SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });
    ui.add(std::move(menuBtn));
}

void SpecialScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}
