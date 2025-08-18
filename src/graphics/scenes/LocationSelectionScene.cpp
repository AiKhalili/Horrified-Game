#include "graphics/scenes/LocationSelectionScene.hpp"
#include "saves/SaveManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/scenes/SceneManager.hpp"
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
#include <fstream>
#include <sstream>
#include "core/Game.hpp"
#include "raymath.h"

void LocationSelectionScene::setData(const std::vector<Location *> &Locations, const std::string &newkey)
{
    Sentlocations = Locations;
    scenekey = newkey;
    if (firstDeserialize == false)
    {
        locationSelect = nullptr;
    }
}

void LocationSelectionScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("LocationSelection", "assets/images/background/location_selection.jpg");

    normalFont = LoadFontEx("assets/fonts/simple.ttf", 30, 0, 0);
    SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);

    locationFont = LoadFontEx("assets/fonts/arial.ttf", 30, 0, 0);
    SetTextureFilter(locationFont.texture, TEXTURE_FILTER_BILINEAR);

    locations = {
        {{62, 225}, 40, "Cave"},
        {{195, 210}, 55, "Camp"},
        {{340, 180}, 60, "Precinct"},
        {{480, 153}, 55, "Inn"},
        {{645, 175}, 55, "Barn"},
        {{788, 182}, 50, "Dungeon"},
        {{716, 328}, 55, "Tower"},
        {{545, 340}, 60, "Theatre"},
        {{768, 474}, 57, "Docks"},
        {{240, 465}, 60, "Mansion"},
        {{87, 530}, 55, "Abbey"},
        {{62, 682}, 50, "Crypt"},
        {{190, 667}, 60, "Museum"},
        {{324, 808}, 47, "Hospital"},
        {{387, 698}, 60, "Church"},
        {{508, 795}, 60, "Graveyard"},
        {{690, 790}, 60, "Institute"},
        {{590, 666}, 60, "Laboratory"},
        {{473, 563}, 54, "Shop"}};

    createLabels();
    createButtons();
}

void LocationSelectionScene::onExit()
{
    UnloadFont(normalFont);
    UnloadFont(locationFont);
    locationSelect = nullptr;
    Sentlocations.clear();
    ui.clear();
}

void LocationSelectionScene::handleClickOnLocation()
{
    if (!hoveredLocation.empty() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        for (auto loc : Sentlocations)
        {
            if (loc->get_name() == hoveredLocation)
            {
                locationSelect = loc;
                AudioManager::getInstance().playSoundEffect("click");
                break;
            }
        }
    }
}

void LocationSelectionScene::checkLocationHover()
{
    hoveredLocation.clear();
    Vector2 mousePos = GetMousePosition();

    for (const auto &loc : locations)
    {
        if (!isLocationActive(loc.locatonName))
            continue;

        float dist = Vector2Distance(mousePos, loc.posision);
        if (dist < loc.radius)
        {
            hoveredLocation = loc.locatonName;
            break;
        }
    }
}

void LocationSelectionScene::update(float)
{
    checkLocationHover();
    handleClickOnLocation();

    AudioManager::getInstance().update();
    ui.update();
}

void LocationSelectionScene::drawGlow()
{
    if (!hoveredLocation.empty())
    {
        for (const auto &loc : locations)
        {
            if (loc.locatonName == hoveredLocation)
            {
                Color neonGreen = {57, 255, 20, 255};
                for (int i = 0; i < 20; i++)
                {
                    float radius = loc.radius + i;
                    float alpha = 1.0f - (float)i / 20;
                    Color ringColor = {neonGreen.r, neonGreen.g, neonGreen.b, (unsigned char)(alpha * 255)};
                    DrawCircleLinesV(loc.posision, radius, ringColor);
                }
                break;
            }
        }
    }

    if (locationSelect)
    {
        for (const auto &loc : locations)
        {
            if (loc.locatonName == locationSelect->get_name())
            {
                Color neonGreen = {57, 255, 20, 255};
                for (int i = 0; i < 20; i++)
                {
                    float radius = loc.radius + i;
                    float alpha = 1.0f - (float)i / 20;
                    Color ringColor = {neonGreen.r, neonGreen.g, neonGreen.b, (unsigned char)(alpha * 255)};
                    DrawCircleLinesV(loc.posision, radius, ringColor);
                }
                break;
            }
        }
    }
}

void LocationSelectionScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    float scale = 0.5f;
    Vector2 position = {
        (screenWidth - background.width * scale) / 2.0f,
        (screenHeight - background.height * scale) / 2.0f};

    DrawTextureEx(background, position, 0.0f, scale, WHITE);

    drawInactiveLocations();
    drawGlow();
    ui.render();

    if (!hoveredLocation.empty())
    {
        Vector2 pos = {(float)GetMouseX() + 15, (float)GetMouseY() - 20};
        DrawTextEx(locationFont, hoveredLocation.c_str(), pos, 28, 1, {0, 0, 0, 255});
    }

    EndDrawing();
}

Location *LocationSelectionScene::getSelectedLocation()
{
    return locationSelect;
}

bool LocationSelectionScene::isLocationActive(const std::string &name)
{
    for (auto *loc : Sentlocations)
    {
        if (loc->get_name() == name)
            return true;
    }
    return false;
}

void LocationSelectionScene::drawInactiveLocations()
{
    for (const auto &loc : locations)
    {
        if (!isLocationActive(loc.locatonName))
        {
            Color overlay = {120, 120, 120, 150};
            DrawCircleV(loc.posision, loc.radius, overlay);
        }
    }
}

void LocationSelectionScene::createLabels()
{
    const char *text = "Please Select Location";
    int fontSize = 50;
    Vector2 textSize = MeasureTextEx(normalFont, text, fontSize, 1);

    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    auto selectText = std::make_unique<UILabel>(
        Vector2{screenWidth * 0.65f, screenHeight * 0.05f}, text, fontSize, 0.0f, WHITE);
    selectText->setFont(normalFont);
    ui.add(std::move(selectText));

    auto tempLabel = std::make_unique<UILabel>(Vector2{1250, 875}, "", 30, 3.0f, WHITE, WHITE, true);
    tempLabel->setFont(locationFont);

    errorLabel = tempLabel.get();

    ui.add(std::move(tempLabel));

    const std::vector<std::string> lines = {
        "Locations that you are not allowed to go to are inactive",
        "and have a gray halo, and you cannot select them to move to.",
        "However, permitted locations are colored",
        "and active, and you can select the location",
        "you want to move to and then click the Submit button."};

    int font1Size = 30;
    float startY = screenHeight * 0.28f;
    float offset = screenHeight * 0.06f;

    auto addLabel = [this, font1Size](Vector2 pos, const std::string &text)
    {
        auto label = std::make_unique<UILabel>(pos, text.c_str(), font1Size, 0.0f, WHITE);
        label->setFont(normalFont);
        ui.add(std::move(label));
    };

    for (int i = 0; i < (int)lines.size(); i++)
    {
        addLabel({screenWidth * 0.56f, startY + i * offset}, lines[i]);
    }
}

void LocationSelectionScene::createButtons()
{
    Color labelcolor = {0, 51, 102, 255};
    Color textcolor = WHITE;
    Color clickcolor = {192, 192, 192, 255};

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1000, 600, 200, 60}, "Main Menu", 40, textcolor, labelcolor, clickcolor, textcolor);
    menuBtn->setFont(normalFont);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        Game::getInstance().reset();
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });

    ui.add(std::move(menuBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1000, 680, 200, 60}, "Save", 40, textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(normalFont);
    saveBtn->setOnClick([this]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot("LocationSelectionScene");
    const std::string msg = "The game was successfully saved!";
                            showErrorMessage(msg); });

    ui.add(std::move(saveBtn));

    auto boardBtn = std::make_unique<UIButton>(Rectangle{1250, 600, 200, 60}, "Board Scene", 40, textcolor, labelcolor, clickcolor, textcolor);
    boardBtn->setFont(normalFont);
    boardBtn->setOnClick([]()
                         {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });

    ui.add(std::move(boardBtn));

    auto nonBtn = std::make_unique<UIButton>(
        Rectangle{1250, 680, 200, 60}, "None", 40,
        textcolor, labelcolor, clickcolor, textcolor);
    nonBtn->setFont(normalFont);
    nonBtn->setOnClick([this]()
                       {
        AudioManager::getInstance().playSoundEffect("click");
        locationSelect = nullptr;
        SceneDataHub::getInstance().setSelectedLocation({}); 
        SceneManager::getInstance().goTo(scenekey); });
    ui.add(std::move(nonBtn));

    auto submitBtn = std::make_unique<UIButton>(
        Rectangle{1125, 760, 200, 60}, "Submit", 40,
        textcolor, labelcolor, clickcolor, textcolor);
    submitBtn->setFont(normalFont);
    submitBtn->setOnClick([this]()
                          {
        AudioManager::getInstance().playSoundEffect("click");
        SceneDataHub::getInstance().setSelectedLocation(this->getSelectedLocation());
        SceneManager::getInstance().goTo(scenekey); });
    ui.add(std::move(submitBtn));
}

void LocationSelectionScene::showErrorMessage(const std::string &msg)
{
    if (!errorLabel)
        return;

    errorLabel->setText(msg);
}

std::vector<Location *> LocationSelectionScene::getSentlocations() { return Sentlocations; }
std::string LocationSelectionScene::getscenekey() { return scenekey; }

void LocationSelectionScene::serialize(const std::string &filename)
{
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open())
        return;

    outFile << "SceneKey:LocationSelectionScene\n";
    outFile << "SceneData:\n";

    outFile << "SceneReturnKey:" << scenekey << "\n";

    outFile << "HoveredLocation:" << hoveredLocation << "\n";

    outFile << "SelectedLocation:" << (locationSelect ? locationSelect->get_name() : "") << "\n";

    outFile << "SentLocationCount:" << Sentlocations.size() << "\n";
    for (auto *loc : Sentlocations)
    {
        outFile << "SentLocation:" << loc->get_name() << "\n";
    }

    outFile << "LocationCount:" << locations.size() << "\n";
    for (const auto &loc : locations)
    {
        outFile << "Location:" << loc.locatonName << ","
                << loc.posision.x << "," << loc.posision.y << ","
                << loc.radius << "\n";
    }

    outFile.close();
}

void LocationSelectionScene::deserialize(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        return;

    std::string line;
    bool isLocationSelectionScene = false;
    bool inSceneData = false;
    std::vector<std::string> sentLocationNames;

    while (std::getline(inFile, line))
    {
        if (!isLocationSelectionScene)
        {
            if (line == "SceneKey:LocationSelectionScene")
            {
                isLocationSelectionScene = true;
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

        std::stringstream ss(line);
        std::string key;
        if (std::getline(ss, key, ':'))
        {
            std::string value;
            std::getline(ss, value);

            if (key == "SceneReturnKey")
            {
                scenekey = value;
            }
            else if (key == "HoveredLocation")
            {
                hoveredLocation = value;
            }

            else if (key == "SelectedLocation")
            {
                

                if (value.empty() || value == "None")
                    continue;

                locationSelect = nullptr;
                for (auto *loc : Map::get_instanse()->getAllLocations())
                {
                    if (loc->get_name() == value)
                    {
                        locationSelect = loc;
                        SceneDataHub::getInstance().setSelectedLocation(locationSelect);
                        break;
                    }
                }
            }

            else if (key == "SentLocationCount")
            {
                sentLocationNames.clear();
                sentLocationNames.reserve(std::stoi(value));
            }
            else if (key == "SentLocation")
            {
                sentLocationNames.push_back(value);
            }
            else if (key == "LocationCount")
            {
                locations.clear();
                locations.reserve(std::stoi(value));
            }
            else if (key == "Location")
            {
                std::stringstream locStream(value);
                std::string name, xStr, yStr, rStr;

                std::getline(locStream, name, ',');
                std::getline(locStream, xStr, ',');
                std::getline(locStream, yStr, ',');
                std::getline(locStream, rStr, ',');

                LocationMarker loc;
                loc.locatonName = name;
                loc.posision.x = std::stof(xStr);
                loc.posision.y = std::stof(yStr);
                loc.radius = std::stof(rStr);

                locations.push_back(loc);
            }
        }
    }

        Sentlocations.clear();
        for (const auto &name : sentLocationNames)
        {
            for (auto *loc : Map::get_instanse()->getAllLocations())
            {
                if (loc->get_name() == name)
                {
                    Sentlocations.push_back(loc);
                    break;
                }
            }
        }
    

    if (!locationSelect && !hoveredLocation.empty())
    {
        for (auto *loc : Sentlocations)
        {
            if (loc->get_name() == hoveredLocation)
            {
                locationSelect = loc;
                break;
            }
        }
    }

    if (hoveredLocation.empty() && locationSelect)
    {
        hoveredLocation = locationSelect->get_name();
    }

    firstDeserialize = false;
    inFile.close();
}
