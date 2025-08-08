#include "graphics/scenes/AdvanceScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include "graphics/scenes/ItemSelectionScene.hpp"
#include "graphics/scenes/MonsterSelectionScene.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "core/Game.hpp"

void AdvanceScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("AdvanceScene", "assets/images/background/advance_scene.png");
    font = LoadFont("assets/fonts/simple.ttf");
    errorFont = LoadFont("assets/fonts/arial.ttf");

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(errorFont.texture, TEXTURE_FILTER_BILINEAR);

    hero = Game::getInstance().getCurrentHero();

    createLabels();
    createButtons();

    itemselect = SceneDataHub::getInstance().getSelectedItems();
    monselect = SceneDataHub::getInstance().getSelectedMonster();

    if (errorLabel)
    {
        errorLabel->setText("");
    }

    AudioManager::getInstance().stopMusic();
    AudioManager::getInstance().playAdDefSelectMusic();
}

void AdvanceScene::onExit()
{
    UnloadFont(font);
    UnloadFont(errorFont);
    itemselect.clear();
    monselect = nullptr;
    ui.clear();
    AudioManager::getInstance().stopAdDefSelectMusic();
    AudioManager::getInstance().playMusic();
}

void AdvanceScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    if (abilityEffectActive)
    {
        abilityEffectTimer -= deltaTime;
        for (auto &p : abilityParticles)
        {
            p.position.x += p.velocity.x * deltaTime * 60;
            p.position.y += p.velocity.y * deltaTime * 60;
            p.alpha -= deltaTime * 1.5f;
        }
        abilityParticles.erase(std::remove_if(abilityParticles.begin(), abilityParticles.end(),
                                              [](const Particle &p)
                                              { return p.alpha <= 0.0f; }),
                               abilityParticles.end());
        if (abilityEffectTimer <= 0.0f)
        {
            abilityEffectActive = false;
            abilityParticles.clear();
        }
    }
    ui.update();
}

void AdvanceScene::render()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);

    if (abilityEffectActive)
    {
        for (auto &p : abilityParticles)
        {
            Color c = {74, 201, 255, (unsigned char)(p.alpha * 255)};
            DrawCircleV(p.position, 6, c);
        }
    }

    ui.render();
}

void AdvanceScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}

void AdvanceScene::createLabels()
{
    const char *text = "Advance Scene";
    int fontSize = 75;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color textcolor = {255, 225, 130, 255};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 23}, text, fontSize, 0.0f, textcolor);
    selectText->setFont(font);
    ui.add(std::move(selectText));

    auto tempLabel = std::make_unique<UILabel>(Vector2{800, 800}, "", 30, 3.0f, WHITE, WHITE, true);
    tempLabel->setFont(errorFont);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));
}

void AdvanceScene::createButtons()
{
    float buttonWidth = 250.0f;
    float buttonHeight = 70.0f;
    float gapY = 50.0f;

    float centerX = GetScreenWidth() / 2.0f;
    float leftX = centerX - buttonWidth - 30.0f;
    float rightX = centerX + 30.0f;
    float topY = GetScreenHeight() / 2.0f - (buttonHeight * 1.5f + gapY) + 10.0f;

    int fontsize = 40;

    Color textcolor = {255, 255, 200, 255};
    Color labelcolor = {20, 40, 50, 255};
    Color clickcolor = {40, 70, 90, 180};
    Color clickcolor2 = {230, 230, 180, 180};

    if (auto *sci = dynamic_cast<Scientist *>(hero))
    {
        Color abilityTextColor = {255, 255, 210, 255};
        Color abilityLabelColor = {20, 40, 50, 255};
        Color abilityClickColor = {74, 201, 255, 180};
        Color abilityClickTextColor = {255, 255, 255, 255};

        auto abilityBtn = std::make_unique<UIButton>(
            Rectangle{centerX - buttonWidth / 2, topY - (gapY + buttonHeight),
                      buttonWidth, buttonHeight},
            "Activate Ability", fontsize,
            abilityTextColor, abilityLabelColor, abilityClickColor, abilityClickTextColor);
        abilityBtn->setFont(font);
        abilityBtn->setOnClick([this, sci]()
                               {
        AudioManager::getInstance().playSoundEffect("magic");
        sci->activateAbility();

        abilityEffectActive = true;
        abilityEffectTimer = 0.6f;
        abilityParticles.clear();
        for (int i = 0; i < 40; i++) {
            float angle = GetRandomValue(0, 360) * DEG2RAD;
            float speed = GetRandomValue(100, 300) / 100.0f;
            Particle p;
            p.position = {(float)GetScreenWidth()/2, (float)GetScreenHeight()/2};
            p.velocity = {cosf(angle)*speed, sinf(angle)*speed};
            p.lifetime = 0.6f;
            p.alpha = 1.0f;
            abilityParticles.push_back(p);
        } });
        ui.add(std::move(abilityBtn));
    }

    auto saveBtn = std::make_unique<UIButton>(Rectangle{leftX, topY + gapY + buttonHeight, buttonWidth, buttonHeight}, "Save", fontsize,
                                              textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");
                            SaveManager::getInstance().saveGameToSlot("AdvanceScene");
                           const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg); 
                            SceneDataHub::getInstance().reset(); });
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

    auto itemBtn = std::make_unique<UIButton>(Rectangle{rightX, topY, buttonWidth, buttonHeight}, "Item Select", fontsize,
                                              labelcolor, textcolor, clickcolor2, textcolor);
    itemBtn->setFont(font);
    itemBtn->setOnClick([this]()
                        {
                           AudioManager::getInstance().playSoundEffect("click");
                            auto& itemselect = SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE);
                            itemselect.setData(hero->getItems(), "AdvanceScene");
                           SceneManager::getInstance().goTo(SceneKeys::ITEM_SELECTION_SCENE); });
    ui.add(std::move(itemBtn));

    auto monBtn = std::make_unique<UIButton>(Rectangle{leftX, topY, buttonWidth, buttonHeight}, "Monster Select", fontsize,
                                             labelcolor, textcolor, clickcolor2, textcolor);
    monBtn->setFont(font);
    monBtn->setOnClick([this]()
                       {
                           AudioManager::getInstance().playSoundEffect("click");
                            auto& monselect = SceneManager::getInstance().getScene<MonsterSelectionScene>(SceneKeys::MONSTER_SELECTION_SCENE);
                            monselect.setDate(Game::getInstance().getMonsters(), "AdvanceScene");
                           SceneManager::getInstance().goTo(SceneKeys::MONSTER_SELECTION_SCENE); });
    ui.add(std::move(monBtn));

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
                                              "Advance", fontsize, labelcolor, textcolor, clickcolor2, textcolor);
    pickBtn->setFont(font);
    pickBtn->setOnClick([this]()
                        {
    AudioManager::getInstance().playSoundEffect("click");

    try
    {
        if (itemselect.empty() || monselect == nullptr)
        {
            showErrorMessage("You cannot advance if no items are selected or no monster is selected.");
            return;
        }

        Game::getInstance().handleAdvance(monselect, itemselect);

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
