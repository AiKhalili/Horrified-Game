#include "graphics/scenes/ShowActionsScene.hpp"
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

void ShowActionsScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("ShowActions", "assets/images/background/show_actions.png");
    font = LoadFont("assets/fonts/simple.ttf");
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

void ShowActionsScene::craeteLabels()
{
    auto selectText = std::make_unique<UILabel>(
        Vector2{800, 40}, "PLEASE SELECT ACTION", 35, 0.0f, WHITE);
    selectText->setFont(font);
    ui.add(std::move(selectText));
}

void ShowActionsScene::createButtons()
{
    float leftX = 100;
    float rightX = 600;
    float startY = 150;
    float gap = 80;
    float width = 200;
    float height = 60;

    auto makeButton = [&](const std::string &text, float x, int index, std::function<void()> action)
    {
        Rectangle bounds = {x, startY + gap * index, width, height};
        auto btn = std::make_unique<UIButton>(bounds, text, 32, WHITE, DARKGREEN, GREEN);
        btn->setFont(font);
        btn->setOnClick([action]()
                        {
            AudioManager::getInstance().playSoundEffect("click");
            action(); });
        ui.add(std::move(btn));
    };

    // ۴ دکمه سمت چپ
     makeButton("Move", leftX, 0, std::bind(&ShowActionsScene::openMoveAction, this));
     makeButton("Pick Up", leftX, 1, std::bind(&ShowActionsScene::openPickUpAction, this));
     makeButton("Defeat", leftX, 2, std::bind(&ShowActionsScene::openDefeatAction, this));
     makeButton("Help", leftX, 3, std::bind(&ShowActionsScene::openHelpAction, this));

    // ۴ دکمه سمت راست
    makeButton("Guide", rightX, 0, std::bind(&ShowActionsScene::openGuideAction, this));
    makeButton("Advance", rightX, 1, std::bind(&ShowActionsScene::openAdvanceAction, this));
    makeButton("Use Perk", rightX, 2, std::bind(&ShowActionsScene::openUsePerkAction, this));
    makeButton("Special", leftX, 3, std::bind(&ShowActionsScene::openSpecialAction, this));
    // makeButton("Quit", rightX, 3, std::bind(&ShowActionsScene::openAction9, this));
}

void ShowActionsScene::openMoveAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::MoveScene);
}

void ShowActionsScene::openPickUpAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::PickupScene);
}

void ShowActionsScene::openDefeatAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::DefeatScene);
}

void ShowActionsScene::openHelpAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::HelpScene);
}

void ShowActionsScene::openGuideAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::GuideScene);
}

void ShowActionsScene::openAdvanceAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::AdvanceScene);
}

void ShowActionsScene::openUsePerkAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::UsePerkScene);
}

void ShowActionsScene::openSpecialAction()
{
    // SceneManager::getInstance().goTo(SceneKeys::SpecialScene);
}
