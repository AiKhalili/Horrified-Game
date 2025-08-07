#include "graphics/scenes/ShowActionsScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include "graphics/scenes/PerkSelectionScene.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include "core/Game.hpp"

void ShowActionsScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("ShowActions", "assets/images/background/show_actions.png");
    font = LoadFont("assets/fonts/simple.ttf");

    createLabels();
    createButtons();

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void ShowActionsScene::onExit()
{
    UnloadFont(font);
    ui.clear();
}

void ShowActionsScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void ShowActionsScene::render()
{
    ClearBackground(BLACK);

    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0}, 0.0f, WHITE);

    ui.render();
}

void ShowActionsScene::createLabels()
{
    const char *text = "Please Select Action";
    int fontSize = 70;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 50}, text, fontSize, 0.0f, WHITE);
    selectText->setFont(font);
    ui.add(std::move(selectText));


    auto tempLabel = std::make_unique<UILabel>(Vector2{800, 800}, "", 45, 3.0f, WHITE, WHITE, true);
    tempLabel->setFont(font);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));
}

void ShowActionsScene::createButtons()
{
    float width = 300;
    float height = 90;
    float gap = 40;
    float columnGap = 120;

    float totalWidth = 2 * width + columnGap;
    float startXLeft = (1600 - totalWidth) / 2.0f;
    float startXRight = startXLeft + width + columnGap;

    float blockHeight = (4 * height) + (3 * gap);
    float startY = (900 - blockHeight) / 2.0f;

    Color darkGreenGray = {25, 70, 70, 255}; 
    Color mediumGreenGray = {40, 100, 100, 255};

    auto makeButton = [&](const std::string &text, float x, int index, std::function<void()> action)
    {
        Rectangle bounds = {x, startY + (height + gap) * index, width, height};
        auto btn = std::make_unique<UIButton>(bounds, text, 50, WHITE, darkGreenGray, mediumGreenGray, WHITE);
        btn->setFont(font);
        btn->setOnClick([action]()
                        {
            AudioManager::getInstance().playSoundEffect("click");
            action(); });
        ui.add(std::move(btn));
    };

    // ستون چپ
    makeButton("Move", startXLeft, 0, std::bind(&ShowActionsScene::openMoveAction, this));
    makeButton("Pick Up", startXLeft, 1, std::bind(&ShowActionsScene::openPickUpAction, this));
    makeButton("Defeat", startXLeft, 2, std::bind(&ShowActionsScene::openDefeatAction, this));
    makeButton("Help", startXLeft, 3, std::bind(&ShowActionsScene::openHelpAction, this));

    // ستون راست
    makeButton("Guide", startXRight, 0, std::bind(&ShowActionsScene::openGuideAction, this));
    makeButton("Advance", startXRight, 1, std::bind(&ShowActionsScene::openAdvanceAction, this));
    makeButton("Use Perk", startXRight, 2, std::bind(&ShowActionsScene::openUsePerkAction, this));

    if (Game::getInstance().shouldShowSpecialAcion() == true)
        makeButton("Special", startXRight, 3, std::bind(&ShowActionsScene::openSpecialAction, this));

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1450, 840, 120, 40}, "Main Menu", 20, WHITE, DARKGRAY, GRAY, WHITE);
    menuBtn->setFont(font);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });

    ui.add(std::move(menuBtn));

    auto backBtn = std::make_unique<UIButton>(Rectangle{1450, 790, 120, 40}, "Back", 20, WHITE, DARKGRAY, GRAY, WHITE);
    backBtn->setFont(font);
    backBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });

    ui.add(std::move(backBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1450, 740, 120, 40}, "Save", 20, WHITE, DARKGRAY, GRAY, WHITE);
    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot("ShowActionsScene");
        const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg); 
    });

    ui.add(std::move(saveBtn));
}

void ShowActionsScene::openMoveAction()
{
    SceneManager::getInstance().goTo(SceneKeys::MOVE_SCENE);
}

void ShowActionsScene::openPickUpAction()
{
    SceneManager::getInstance().goTo(SceneKeys::PICK_UP_SCENE);
}

void ShowActionsScene::openDefeatAction()
{
    SceneManager::getInstance().goTo(SceneKeys::DEFEAT_SCENE);
}

void ShowActionsScene::openHelpAction()
{
    SceneManager::getInstance().goTo(SceneKeys::HELP_SCENE);
}

void ShowActionsScene::openGuideAction()
{
    SceneManager::getInstance().goTo(SceneKeys::GUID_SCENE);
}

void ShowActionsScene::openAdvanceAction()
{
    SceneManager::getInstance().goTo(SceneKeys::ADVANCE_SCENE);
}

void ShowActionsScene::openUsePerkAction()
{
    auto& perkselect = SceneManager::getInstance().getScene<PerkSelectionScene>(SceneKeys::PERK_SELECTION_SCENE);
                            perkselect.setData(Game::getInstance().getCurrentHero()->getPerkCard(), "ShowActionsScene");
                           SceneManager::getInstance().goTo(SceneKeys::PERK_SELECTION_SCENE);
}

void ShowActionsScene::openSpecialAction()
{
    SceneManager::getInstance().goTo(SceneKeys::SPECIAL_SCENE);
}

void ShowActionsScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}
