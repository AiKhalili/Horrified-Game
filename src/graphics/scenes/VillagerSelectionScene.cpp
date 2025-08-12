#include "graphics/scenes/VillagerSelectionScene.hpp"
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

void VillagerSelectionScene::setData(const std::vector<Villager *> &Villagers, const std::string &newkey)
{
    villagers = Villagers;
    scenekey = newkey;
}

void VillagerSelectionScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture(
        "VillagerSelection", "assets/images/background/villager_selection.png");
    font = LoadFont("assets/fonts/simple.ttf");
    errorFont = LoadFont("assets/fonts/arial.ttf");

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(errorFont.texture, TEXTURE_FILTER_BILINEAR);

    createLabels();
    createButtons();

    if (villagers.empty())
    {
        creatErroreLabels();
    }

    selected.clear();
    villagerRects.clear();
    villagerRects.reserve(villagers.size());

    villagerTextures.clear();

    loadVillagerTextures();

    int cols = 4;
    int rows = 2;
    float spacing = -200.0f;
    float itemSize = 480.0f;

    float totalWidth = cols * itemSize + (cols - 1) * spacing;
    float startX = (1600.0f - totalWidth) / 2.0f;
    float startY = 80.0f;

    for (int i = 0; i < (int)villagers.size(); ++i)
    {
        int row = i < 4 ? 0 : 1;
        int col = i % 4;

        float x = startX + col * (itemSize + spacing);
        float y = startY + row * (itemSize + 40.0f);

        if (row == 1)
        {
            x += 130;
            y -= 140;
        }

        villagerRects.push_back({x, y, itemSize, itemSize});
    }
}

void VillagerSelectionScene::onExit()
{
    UnloadFont(font);
    UnloadFont(errorFont);
    ui.clear();
    selected.clear();
    villagers.clear();
    villagerTextures.clear();
}

void VillagerSelectionScene::update(float deltaTime)
{
    AudioManager::getInstance().update();

    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        for (size_t i = 0; i < villagerRects.size(); ++i)
        {
            if (CheckCollisionPointRec(mousePos, villagerRects[i]))
            {
                toggleSelection(villagers[i]);
                break;
            }
        }
    }

    ui.update();
}

void VillagerSelectionScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0}, 0.0f, WHITE);

    for (size_t i = 0; i < villagers.size(); ++i)
    {
        Texture2D tex = villagerTextures[i];
        Rectangle dest = villagerRects[i];

        float scaleX = dest.width / (float)tex.width;
        float scaleY = dest.height / (float)tex.height;
        float scale = std::min(scaleX, scaleY);

        float scaledWidth = tex.width * scale;
        float scaledHeight = tex.height * scale;

        Rectangle source = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle target = {
            dest.x + (dest.width - scaledWidth) / 2.0f,
            dest.y + (dest.height - scaledHeight) / 2.0f,
            scaledWidth,
            scaledHeight};

        DrawTexturePro(tex, source, target, Vector2{0, 0}, 0.0f, WHITE);

        if (std::find(selected.begin(), selected.end(), villagers[i]) != selected.end())
        {
            float frameWidth = dest.width * 0.4f;
            float frameHeight = dest.height * 0.6f;
            float frameX = dest.x + (dest.width - frameWidth) / 2.0f;
            float frameY = dest.y + (dest.height - frameHeight) / 2.0f;

            Rectangle frameRect = {frameX, frameY, frameWidth, frameHeight};
            DrawRectangleLinesEx(frameRect, 2, RAYWHITE);
        }
        else if (CheckCollisionPointRec(GetMousePosition(), dest))
        {
            float frameWidth = dest.width * 0.4f;
            float frameHeight = dest.height * 0.6f;
            float frameX = dest.x + (dest.width - frameWidth) / 2.0f;
            float frameY = dest.y + (dest.height - frameHeight) / 2.0f;

            Rectangle hoverRect = {frameX, frameY, frameWidth, frameHeight};
            DrawRectangleLinesEx(hoverRect, 2, GRAY);
        }
    }

    ui.render();
    EndDrawing();
}

void VillagerSelectionScene::createLabels()
{
    const char *text = "Please Select Villager";
    int fontSize = 45;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color textcolor = {232, 213, 183, 255};
    Color labelcolor = {139, 110, 78, 200};
    Color clickcolor = {74, 59, 42, 150};
    Color midCreamBrown = {140, 110, 70, 255};
    Color text1color = {50, 60, 42, 255};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 50}, text, fontSize, 0.0f, text1color);
    selectText->setFont(font);
    ui.add(std::move(selectText));

    auto tempLabel = std::make_unique<UILabel>(Vector2{800, 800}, "", 30, 3.0f, WHITE, WHITE, true);
    tempLabel->setFont(font);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));

}

std::vector<Villager *> &VillagerSelectionScene::getSelectedVillagers()
{
    return selected;
}

void VillagerSelectionScene::loadVillagerTextures()
{
    villagerTextures.clear();
    for (auto *villager : villagers)
    {
        std::string folder;

        folder = "assets/images/Villager/";
        std::string path = folder + villager->getName() + ".png";
        Texture2D tex = TextureManager::getInstance().getOrLoadTexture(villager->getName(), path);
        villagerTextures.push_back(tex);
    }
}

void VillagerSelectionScene::toggleSelection(Villager *villager)
{
    if (scenekey == "GuidScene")
    {
        selected.clear();
        selected.push_back(villager);
        return;
    }

    auto it = std::find(selected.begin(), selected.end(), villager);
    if (it == selected.end())
        selected.push_back(villager);
    else
        selected.erase(it);
}

void VillagerSelectionScene::creatErroreLabels()
{
    const char *text = "There are no  villagers to display";
    int fontSize = 50;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    auto errorText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 450}, text, fontSize, 0.0f, WHITE);
    errorText->setFont(errorFont);
    ui.add(std::move(errorText));
}

void VillagerSelectionScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}

void VillagerSelectionScene::createButtons()
{
    Color textcolor = {232, 213, 183, 255};
    Color labelcolor = {139, 110, 78, 200};
    Color clickcolor = {74, 59, 42, 150};
    Color midCreamBrown = {140, 110, 70, 255};
    Color text1color = {50, 60, 42, 255};

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1450, 840, 120, 40}, "Main Menu", 20, textcolor, labelcolor, clickcolor, textcolor);
    menuBtn->setFont(font);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        Game::getInstance().reset();
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });

    ui.add(std::move(menuBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1450, 740, 120, 40}, "Save", 20, textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot("VillagerSelectionScene"); 
        const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg); });

    ui.add(std::move(saveBtn));

    auto boardBtn = std::make_unique<UIButton>(Rectangle{1450, 790, 120, 40}, "Board Scene", 20, textcolor, labelcolor, clickcolor, textcolor);
    boardBtn->setFont(font);
    boardBtn->setOnClick([]()
                         {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });

    ui.add(std::move(boardBtn));

    auto backBtn = std::make_unique<UIButton>(Rectangle{1450, 690, 120, 40}, "Back", 20, textcolor, labelcolor, clickcolor, textcolor);
    backBtn->setFont(font);
    backBtn->setOnClick([this]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(scenekey); });

    ui.add(std::move(backBtn));

    if (!villagers.empty())
    {

        auto nonBtn = std::make_unique<UIButton>(Rectangle{40, 790, 120, 45}, "Non", 35, textcolor, text1color, clickcolor, midCreamBrown);
        nonBtn->setFont(font);
        nonBtn->setOnClick([this]()
                           {
                           AudioManager::getInstance().playSoundEffect("click");
                           this->selected.clear();
                           SceneDataHub::getInstance().setSelectedVillagers({});
                           SceneManager::getInstance().goTo(scenekey); });

        ui.add(std::move(nonBtn));

        auto submtBtn = std::make_unique<UIButton>(Rectangle{40, 840, 120, 45}, "Submit", 35,
                                                   textcolor, text1color, clickcolor, midCreamBrown);
        submtBtn->setFont(font);
        submtBtn->setOnClick([this]()
                             {
                             AudioManager::getInstance().playSoundEffect("click");
                             SceneDataHub::getInstance().setSelectedVillagers(this->getSelectedVillagers());
                             SceneManager::getInstance().goTo(scenekey); });
        ui.add(std::move(submtBtn));
    }
}

std::string VillagerSelectionScene::getscenekey(){return scenekey;}
std::vector<Villager *> VillagerSelectionScene::getVillagers(){return villagers;}