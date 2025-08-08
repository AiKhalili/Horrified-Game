#include "graphics/scenes/PerkSelectionScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include "core/SceneDataHub.hpp"
#include "graphics/scenes/LocationSelectionScene.hpp"
#include "core/Map.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>
#include "core/Game.hpp"

void PerkSelectionScene::setData(const std::vector<PerkCard> &Perks, const std::string &newkey)
{
    perks = Perks;
    scenekey = newkey;
   // hero = Game::getInstance().getCurrentHero();
}
void PerkSelectionScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture(
        "PerkSelection", "assets/images/background/perk_selection.png");
    font = LoadFont("assets/fonts/simple.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    createLabels();
    createButtons();

    Location *loc = SceneDataHub::getInstance().getSelectedLocation();
    if (loc != nullptr)
    {
        selectedLocations.push_back(loc);
        currentLocationSelectionCount++;

        if (currentLocationSelectionCount < requiredLocationCount)
        {
            handleLocationSelectionRequest();
            return;
        }
    }

    if (isReturningFromLocationSelection)
    {
        createActionButtons();
        isReturningFromLocationSelection = false; // reset
    }
    else
    {
        if (!perks.empty())
            createActionButtons();
        else /*if (!needLocationSelection)*/
            creatErroreLabels();
    }

    perkTextures.clear();
    perkRects.clear();
    loadPerkTextures();

    int maxItems = (int)std::min(perks.size(), size_t(32));
    int cols = 3;
    int rows = (maxItems + cols - 1) / cols;
    float spacingX = 50.0f;
    float spacingY = 40.0f;
    float itemSize = 250.0f;
    float totalWidth = cols * itemSize + (cols - 1) * spacingX;
    float totalHeight = rows * itemSize + (rows - 1) * spacingY;

    float startX = (1600.0f - totalWidth) / 2.0f;
    float startY = (900.0f - totalHeight) / 2.0f;

    perkRects.clear();
    for (int i = 0; i < maxItems; ++i)
    {
        int row = i / cols;
        int col = i % cols;
        Rectangle rect = {
            startX + col * (itemSize + spacingX),
            startY + row * (itemSize + spacingY),
            itemSize,
            itemSize};
        perkRects.push_back(rect);
    }

    AudioManager::getInstance().stopMusic();
    AudioManager::getInstance().playPerkSelectMusic();
}

void PerkSelectionScene::onExit()
{
    UnloadFont(font);
    ui.clear();
    perkRects.clear();
    AudioManager::getInstance().stopPerkSelectMusic();
    AudioManager::getInstance().playMusic();
}

void PerkSelectionScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    hoveredPerkIndex = -1;

    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        for (size_t i = 0; i < perkRects.size(); ++i)
        {
            if (CheckCollisionPointRec(mousePos, perkRects[i]))
            {
                toggleSelection(perks[i]);
                break;
            }
        }
    }

    for (size_t i = 0; i < perkRects.size(); ++i)
    {
        if (CheckCollisionPointRec(mousePos, perkRects[i]))
        {
            hoveredPerkIndex = static_cast<int>(i);
            break;
        }
    }

    ui.update();

    if (isWaitingToGoNextScene)
    {
        timerToNextScene += deltaTime;

        if (timerToNextScene >= waitDuration)
        {
            if (needLocationSelection)
            {
                Game::getInstance().usePerkCard(getSelectedPerkIndex(), selectedLocations);
                selectedLocations.clear();
            }
            else
            {
                Game::getInstance().usePerkCard(getSelectedPerkIndex(), {});
            }

            if (!needLocationSelection)
            {
                int idx = getSelectedPerkIndex();
                if (idx >= 0 && idx < perks.size())
                    perks.erase(perks.begin() + idx);
            }

            SceneManager::getInstance().goTo(scenekey);
            isWaitingToGoNextScene = false;
            timerToNextScene = 0.0f;
        }
    }
}

void PerkSelectionScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0}, 0.0f, WHITE);

    float padding = 6.0f;
    float offsetTopRow = -30.0f;
    float borderExtra = 8.0f;

    for (size_t i = 0; i < perkTextures.size(); ++i)
    {
        float yPos = perkRects[i].y;
        if (i < 3) 
            yPos += offsetTopRow;

        Rectangle targetRect = {
            perkRects[i].x - padding,
            yPos - padding,
            250,
            300
        };

        DrawTexturePro(
            perkTextures[i],
            Rectangle{0, 0, (float)perkTextures[i].width, (float)perkTextures[i].height},
            targetRect,
            Vector2{0, 0}, 0.0f, WHITE);

        Rectangle borderRect = {
            targetRect.x - borderExtra,
            targetRect.y - borderExtra,
            targetRect.width + 2 * borderExtra,
            targetRect.height + 2 * borderExtra
        };

        if ((int)i == hoveredPerkIndex)
            DrawRectangleLinesEx(borderRect, 3, WHITE);

        if (hasPendingSelected && perks[i].getName() == pendingSelectedPerk.getName())
        {
            Color selectedColor = {100, 80, 50, 255};
            DrawRectangleLinesEx(borderRect, 6, selectedColor);
        }
    }

    ui.render();
    EndDrawing();
}

void PerkSelectionScene::toggleSelection(PerkCard &perk)
{
    pendingSelectedPerk = perk;
    hasPendingSelected = true;

    currentSelectedPerk = perk;
    hasCurrentSelected = true;

    selectedLocations.clear();
    currentLocationSelectionCount = 0;

    if (perk.getType() == PerkType::HURRY || perk.getType() == PerkType::REPEL)
    {
        needLocationSelection = true;
        requiredLocationCount = 2;
    }
    else if (perk.getType() == PerkType::VISIT_FROM_THE_DETECTIVE)
    {
        needLocationSelection = true;
        requiredLocationCount = 1;
    }
    else
    {
        needLocationSelection = false;
        requiredLocationCount = 0;
    }
}

void PerkSelectionScene::createLabels()
{
    const char *text = "Please Select Perk Card";
    int fontSize = 55;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color labelcolor = {220, 170, 140, 255};
    Color textcolor = {30, 15, 7, 255};
    Color clickcolor = {235, 195, 165, 255};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 40}, text, fontSize, 0.0f, WHITE);
    selectText->setFont(font);
    ui.add(std::move(selectText));

    auto tempLabel = std::make_unique<UILabel>(Vector2{800, 760}, "", 30, 3.0f, WHITE, WHITE, true);
    tempLabel->setFont(font);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));
}

void PerkSelectionScene::creatErroreLabels()
{
    const char *text = "There are no perk cards to display";
    int fontSize = 40;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color textcolor = {255, 255, 200, 255};
    Color labelcolor = {20, 40, 50, 255};

    auto errorText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 450}, text, fontSize, 0.0f, WHITE, WHITE);
    errorText->setFont(font);
    errorText->enableBackground(labelcolor, 10.0f);
    ui.add(std::move(errorText));
}

void PerkSelectionScene::createActionButtons()
{
    Color labelcolor = {45, 25, 15, 255};
    Color textcolor = WHITE;
    Color clickcolor = {235, 195, 165, 255};

    auto nonBtn = std::make_unique<UIButton>(
        Rectangle{570, 790, 130, 60}, "None", 45,
        labelcolor, textcolor, clickcolor, textcolor);
    nonBtn->setFont(font);
    nonBtn->setOnClick([this]()
                       {
        AudioManager::getInstance().playSoundEffect("click");
        SceneDataHub::getInstance().setSelectedItems({}); 
        SceneManager::getInstance().goTo(scenekey); });
    ui.add(std::move(nonBtn));

    auto submitBtn = std::make_unique<UIButton>(
        Rectangle{900, 790, 160, 60}, "Use Perk", 45,
        labelcolor, textcolor, clickcolor, textcolor);

    submitBtn->setFont(font);
    submitBtn->setOnClick([this]()
                          {
    AudioManager::getInstance().playSoundEffect("click");

    if (!hasPendingSelected)
        return;

    if (needLocationSelection)
    {
        if (currentLocationSelectionCount < requiredLocationCount)
        {
            handleLocationSelectionRequest();
            return;
        }

        selected = pendingSelectedPerk;
        hasSelected = true;

        showDesMessage(selected);
        isWaitingToGoNextScene = true;
        timerToNextScene = 0.0f;
        return;
    }

    selected = pendingSelectedPerk;
    hasSelected = true;

    showDesMessage(selected);
    isWaitingToGoNextScene = true;
    timerToNextScene = 0.0f; });

    ui.add(std::move(submitBtn));
}

void PerkSelectionScene::showDesMessage(PerkCard &perk)
{

    if (!desLabel)
    {
        Color textcolor = {255, 255, 200, 255};
        Color labelcolor = {20, 40, 50, 255};
        auto tempLabel = std::make_unique<UILabel>(Vector2{800, 200}, perk.getDescription(), 40, 10.0f, WHITE, WHITE, true);
        tempLabel->setFont(font);
        tempLabel->enableBackground(labelcolor, 10.0f);
        desLabel = tempLabel.get();
        ui.add(std::move(tempLabel));
    }
    else
    {
        desLabel->setText(perk.getDescription());
    }
}

int PerkSelectionScene::getSelectedPerkIndex()
{
    for (int i = 0; i < perks.size(); ++i)
    {
        if (perks[i].getName() == pendingSelectedPerk.getName())
            return i;
    }
    return -1;
}

void PerkSelectionScene::handleLocationSelectionRequest()
{
    if (!hasCurrentSelected)
        return;

    isReturningFromLocationSelection = true;

    PerkCard &card = currentSelectedPerk;

    if (card.getType() == PerkType::REPEL)
    {
        auto monsters = Game::getInstance().getMonsters();

        if (currentLocationSelectionCount == 0)
        {
            std::vector<Location *> allloc;
            for (auto loc : monsters[0]->get_location()->get_neighbors())
            {
                for (auto secondLoc : loc->get_neighbors())
                {
                    if (std::find(allloc.begin(), allloc.end(), secondLoc) == allloc.end())
                    {
                        if (secondLoc != monsters[0]->get_location())
                            allloc.push_back(secondLoc);
                        else
                            continue;
                    }
                }
            }
            auto &locationselect = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
            locationselect.setData(allloc, "PerkSelectionScene");
        }
        else
        {
            std::vector<Location *> allloc;
            for (auto loc : monsters[1]->get_location()->get_neighbors())
            {
                for (auto secondLoc : loc->get_neighbors())
                {
                    if (std::find(allloc.begin(), allloc.end(), secondLoc) == allloc.end())
                    {
                        if (secondLoc != monsters[1]->get_location())
                            allloc.push_back(secondLoc);
                        else
                            continue;
                    }
                }
            }
            auto &locationselect = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
            locationselect.setData(allloc, "PerkSelectionScene");
        }
    }
    else if (card.getType() == PerkType::HURRY)
    {
        auto heros = Game::getInstance().getHeroes();
        std::vector<Location *> allloc;

        if (currentLocationSelectionCount == 0)
        {
            for (auto loc : heros[0]->getLocation()->get_neighbors())
            {
                for (auto secondLoc : loc->get_neighbors())
                {
                    if (std::find(allloc.begin(), allloc.end(), secondLoc) == allloc.end())
                    {
                        if (secondLoc != heros[0]->getLocation())
                            allloc.push_back(secondLoc);
                        else
                            continue;
                    }
                }
            }
        }
        else
        {
            for (auto loc : heros[1]->getLocation()->get_neighbors())
            {
                for (auto secondLoc : loc->get_neighbors())
                {
                    if (std::find(allloc.begin(), allloc.end(), secondLoc) == allloc.end())
                    {
                        if (secondLoc != heros[1]->getLocation())
                            allloc.push_back(secondLoc);
                        else
                            continue;
                    }
                }
            }
        }
        auto &locationselect = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
        locationselect.setData(allloc, "PerkSelectionScene");
    }
    else if (card.getType() == PerkType::VISIT_FROM_THE_DETECTIVE)
    {
        auto allLocations = Map::get_instanse()->getAllLocations();
        auto &locationselect = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
        locationselect.setData(allLocations, "PerkSelectionScene");
    }

    SceneManager::getInstance().goTo(SceneKeys::LOCATION_SELECTION_SCENE);
}

void PerkSelectionScene::loadPerkTextures()
{
    perkTextures.clear();
    for (auto &perk : perks)
    {
        std::string folder;
        folder = "assets/images/Perk_Cards/";
        std::string path = folder + perk.getName() + ".png";
        Texture2D tex = TextureManager::getInstance().getOrLoadTexture(perk.getName(), path);
        perkTextures.push_back(tex);
    }
}

PerkCard &PerkSelectionScene::getSelectedPerks()
{
    return selected;
}

void PerkSelectionScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}

void PerkSelectionScene::createButtons()
{
    Color labelcolor = {220, 170, 140, 255};
    Color textcolor = {30, 15, 7, 255};
    Color clickcolor = {235, 195, 165, 255};

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
        SaveManager::getInstance().saveGameToSlot("PerkSelectionScene"); 
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
}

std::vector<PerkCard> PerkSelectionScene::getperks() { return perks; }
std::string PerkSelectionScene::getscenekey() { return scenekey; }