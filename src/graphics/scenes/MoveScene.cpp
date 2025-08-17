#include "graphics/scenes/MoveScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include "graphics/scenes/VillagerSelectionScene.hpp"
#include "graphics/scenes/LocationSelectionScene.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/Game.hpp"

void MoveScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("MoveScene", "assets/images/background/move_scene.png");
    font = LoadFont("assets/fonts/simple.ttf");
    errorFont = LoadFont("assets/fonts/arial.ttf");

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(errorFont.texture, TEXTURE_FILTER_BILINEAR);

    hero = Game::getInstance().getCurrentHero();
    location = Game::getInstance().getCurrentHero()->getLocation();

    createLabels();
    createButtons();

    if (!locselect)
        locselect = SceneDataHub::getInstance().getSelectedLocation();
    if (villselect.empty())
        villselect = SceneDataHub::getInstance().getSelectedVillagers();

    if (errorLabel)
    {
        errorLabel->setText("");
    }
}

void MoveScene::onExit()
{
    UnloadFont(font);
    UnloadFont(errorFont);
    villselect.clear();
    locselect = nullptr;
    ui.clear();
}

void MoveScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void MoveScene::render()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);

    ui.render();
}

void MoveScene::createLabels()
{
    const char *text = "Move Scene";
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

void MoveScene::createButtons()
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

    auto villBtn = std::make_unique<UIButton>(Rectangle{rightX, topY, buttonWidth, buttonHeight}, "Villager Select", fontsize,
                                              labelcolor, textcolor, clickcolor2, textcolor);
    villBtn->setFont(font);
    villBtn->setOnClick([this]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");

                            
                            if (locselect == nullptr) {
                                std::string msg = "First select a location to move to!";
                                showErrorMessage(msg);
                                return;
                            }

                            errorLabel->setText("");

                            auto& villagerselect = SceneManager::getInstance().getScene<VillagerSelectionScene>(SceneKeys::VILLAGER_SELECTION_SCENE);
                            villagerselect.setData(location->get_villagers(), "MoveScene");
                            SceneManager::getInstance().goTo(SceneKeys::VILLAGER_SELECTION_SCENE); });
    ui.add(std::move(villBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{leftX, topY + gapY + buttonHeight, buttonWidth, buttonHeight}, "Save", fontsize,
                                              textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");
                            SaveManager::getInstance().saveGameToSlot("MoveScene"); 
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

    auto locBtn = std::make_unique<UIButton>(Rectangle{leftX, topY, buttonWidth, buttonHeight}, "Location Select", fontsize,
                                             labelcolor, textcolor, clickcolor2, textcolor);
    locBtn->setFont(font);
    locBtn->setOnClick([this]()
                       {
                           AudioManager::getInstance().playSoundEffect("click");
                            auto& locationselect = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
                            locationselect.setData(hero->getLocation()->get_neighbors(), "MoveScene");
                           SceneManager::getInstance().goTo(SceneKeys::LOCATION_SELECTION_SCENE); });
    ui.add(std::move(locBtn));

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

    auto moveBtn = std::make_unique<UIButton>(Rectangle{centerX - buttonWidth / 2, topY + 3 * (gapY + buttonHeight), buttonWidth, buttonHeight},
                                              "Move", fontsize, labelcolor, textcolor, clickcolor2, textcolor);
    moveBtn->setFont(font);
    moveBtn->setOnClick([this]()
                        {
    AudioManager::getInstance().playSoundEffect("click");

    try
    {
        if (locselect == nullptr)
        {
            showErrorMessage("Please select a location!");
            return;
        }

        Game::getInstance().handleMove(locselect, villselect);

        showErrorMessage("");
        SceneDataHub::getInstance().reset();
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    }
    catch (const GameException &ex)
    {
        showErrorMessage(ex.what());
    } });
    ui.add(std::move(moveBtn));
}

void MoveScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}

void MoveScene::serialize(const std::string &filename)
{
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open())
        return;

    outFile << "SceneKey:MoveScene\n";
    outFile << "SceneData:\n";

    locselect = SceneDataHub::getInstance().getSelectedLocation();
    villselect = SceneDataHub::getInstance().getSelectedVillagers();

    if (locselect)
        outFile << "SelectedLocation:" << locselect->get_name() << "\n";
    else
        outFile << "SelectedLocation:None\n";

    outFile << "SelectedVillagers:";

    if (!villselect.empty())
    {
        for (size_t i = 0; i < villselect.size(); ++i)
        {
            if (villselect[i])
                outFile << villselect[i]->getName();
            else
                outFile << "None";

            if (i != villselect.size() - 1)
                outFile << ",";
        }
    }
    else
    {
        outFile << "None";
    }
    outFile << "\n";

    outFile.close();
}

void MoveScene::deserialize(const std::string &filename)
{
    hero = Game::getInstance().getCurrentHero();
    location = Game::getInstance().getCurrentHero()->getLocation();

    std::ifstream inFile(filename);
    if (!inFile.is_open())
        return;

    std::string line;
    bool isMoveScene = false;
    bool inSceneData = false;

    while (std::getline(inFile, line))
    {
        if (!isMoveScene)
        {
            if (line == "SceneKey:MoveScene")
            {
                isMoveScene = true;
            }
            continue;
        }

        if (line == "SceneData:")
        {
            inSceneData = true;
            continue;
        }

        if (!inSceneData)
            continue;

        auto pos = line.find(':');
        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "SelectedLocation")
        {
            if (value != "None")
            {
                for (auto *loc : hero->getLocation()->get_neighbors())
                {
                    if (loc->get_name() == value)
                    {
                        locselect = loc;
                        SceneDataHub::getInstance().setSelectedLocation(loc);
                        break;
                    }
                }
            }
        }
        else if (key == "SelectedVillagers")
        {
            villselect.clear();
            if (value != "None")
            {
                std::stringstream villSS(value);
                std::string villName;
                while (std::getline(villSS, villName, ','))
                {
                    for (auto *vill : Map::get_instanse()->getAllVillagers())
                    {
                        if (vill->getName() == villName)
                        {
                            villselect.push_back(vill);
                            break;
                        }
                    }
                }

                SceneDataHub::getInstance().setSelectedVillagers(villselect);
            }
        }
    }

    inFile.close();
}
