#include "graphics/scenes/LocationInfoScene.hpp"
#include "graphics/TextureManager.hpp"
#include "core/Dracula.hpp"
#include "core/Hero.hpp"
#include "core/Villager.hpp"
#include "core/Monster.hpp"
#include "Item.hpp"
#include "core/Game.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "saves/SaveManager.hpp"
#include <fstream>
#include <sstream>

void LocationInfoScene::onEnter()
{
    AudioManager::getInstance().setSoundVolume(10.0f);
    AudioManager::getInstance().playSoundEffect("door");
    AudioManager::getInstance().setSoundVolume(0.5f);

    loadTextures();

    font = LoadFontEx("assets/fonts/simple.ttf", 70, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    AudioManager::getInstance().playMusic();
    createUI();
}

void LocationInfoScene::onExit()
{
    UnloadFont(font);
    ui.clear();
    itemTextures.clear();
    heroTextures.clear();
    monsterTextures.clear();
    villagerTextures.clear();
    currentLocation = nullptr;
}

void LocationInfoScene::setLocation(Location *loc)
{
    currentLocation = loc;
}

void LocationInfoScene::loadTextures()
{
    auto &tm = TextureManager::getInstance();
    Texture2D tex;

    auto monsters = currentLocation->get_monsters();
    auto heroes = currentLocation->get_heroes();
    auto villagers = currentLocation->get_villagers();
    auto items = currentLocation->get_items();

    background = tm.getOrLoadTexture("wooden", "assets/images/background/wooden.png");

    for (const auto &h : heroes)
    {
        std::string name = h->getClassName();
        std::string path = "assets/images/heroes/" + name + ".png";
        tex = tm.getOrLoadTexture(name, path);
        SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
        heroTextures.push_back(tex);
    }

    for (const auto &m : monsters)
    {
        std::string name = m->get_name();
        std::string path = "assets/images/Monsters/" + name + ".png";
        tex = tm.getOrLoadTexture(name, path);
        SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
        monsterTextures.push_back(tex);
    }

    for (const auto &v : villagers)
    {
        std::string name = v->getName();
        std::string path = "assets/images/Villager/" + name + ".png";
        tex = tm.getOrLoadTexture(name, path);
        SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
        villagerTextures.push_back(tex);
    }

    for (const auto &i : items)
    {
        std::string name = i->get_name();
        std::string colorFolder = i->get_color_to_string();

        std::string path = "assets/images/Items/" + colorFolder + "/" + name + ".png";
        Texture2D tex = TextureManager::getInstance().getOrLoadTexture(name, path);
        SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
        itemTextures.push_back({tex, i->get_color()});
    }

    std::sort(itemTextures.begin(), itemTextures.end(), [](const ItemTex &a, const ItemTex &b)
              { return (int)a.color < (int)b.color; });
}

void LocationInfoScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void LocationInfoScene::createUI()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    Color titleColor = {255, 230, 180, 255};

    if (currentLocation)
    {
        Vector2 titlePos = {100, 50};

        auto locLabel = std::make_unique<UILabel>(
            titlePos, currentLocation->get_name(), 90, 0.0f, titleColor, titleColor);

        locLabel->setFont(font);
        locLabel->enableBackground({50, 30, 20, 200}, 25.0f);
        ui.add(std::move(locLabel));
    }

    float btnWidth = 150;
    float btnHeight = 45;
    float btnMargin = 20;
    float btnX = screenW - btnWidth - 40;

    Rectangle backBounds = {btnX, screenH - btnHeight * 2 - btnMargin * 2, btnWidth, btnHeight};

    auto backBtn = std::make_unique<UIButton>(
        backBounds, "Back", 28, WHITE,
        Color{101, 67, 33, 255}, Color{50, 30, 20, 200}, Color{100, 50, 10, 255});

    backBtn->setFont(font);
    backBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });
    ui.add(std::move(backBtn));

    Rectangle saveBounds = {btnX, screenH - btnHeight - btnMargin, btnWidth, btnHeight};

    auto saveBtn = std::make_unique<UIButton>(
        saveBounds, "Save", 28, WHITE,
        Color{101, 67, 33, 255},
        Color{50, 30, 20, 200},
        Color{100, 50, 10, 255});

    saveBtn->setFont(font);
    saveBtn->setOnClick([this]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");
                            SaveManager::getInstance().saveGameToSlot(SceneKeys::LOCATION_INFO_SCENE);
                                                    Vector2 pos = {780,800};
                            auto saveLabel = std::make_unique<UILabel>(pos,"The game was successfully saved!",
                                                                    45,3.0f,WHITE,WHITE,true);
                            saveLabel->setFont(font);
                            ui.add(std::move(saveLabel)); });

    ui.add(std::move(saveBtn));
}

void LocationInfoScene::drawCoffin()
{
    if (!currentLocation)
    {
        return;
    }

    auto monsters = Game::getInstance().getMonsters();
    Dracula *dracula = nullptr;

    for (auto *monster : monsters)
    {
        dracula = dynamic_cast<Dracula *>(monster);
        if (dracula)
        {
            break;
        }
    }

    if (!dracula)
    {
        return;
    }

    std::string locName = currentLocation->get_name();

    if (!dracula->isCoffinLocation(locName))
    {
        return;
    }

    bool broken = dracula->isCoffinDestroyed(locName);

    Texture2D tex = broken ? TextureManager::getInstance().getOrLoadTexture("SmashedCoffin", "assets/images/Coffins/SmashedCoffin.png") : TextureManager::getInstance().getOrLoadTexture("Coffin", "assets/images/Coffins/Coffin.png");

    float targetWidth = 200.0f;
    float scale = targetWidth / tex.width;
    float targetHeght = tex.height * scale;

    Vector2 pos = {0.0f, 0.0f};

    DrawTextureEx(tex, pos, 0.0, scale, WHITE);
    return;
}

void LocationInfoScene::drawHeroAndMonster()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    float startY = 200;
    float padding = 20.0f;

    float posX = 130;

    for (int i = 0; i < heroTextures.size() && i < 2; ++i)
    {
        Texture2D &tex = heroTextures[i];
        float scale = 220.0f / tex.width;
        float drawW = tex.width * scale;
        float drawH = tex.height * scale;
        float posY = startY + i * (drawH + padding);
        DrawTextureEx(tex, {posX, posY}, 0.0f, scale, WHITE);
    }

    Monster *frenzy = Game::getInstance().getFrenzyMonster();
    auto monsters = currentLocation->get_monsters();

    posX = 380;

    for (int i = 0; i < monsterTextures.size() && i < 2; ++i)
    {
        if (!monsters[i] || monsters[i]->is_defeated())
        {
            continue;
        }

        Texture2D &tex = monsterTextures[i];
        float scale = 200.0f / tex.width;
        float drawW = tex.width * scale;
        float drawH = tex.height * scale;
        float posY = startY + i * (drawH + padding);

        if (monsters[i]->get_name() == frenzy->get_name())
        {
            float circleX = posX + drawW / 2.0f;
            float circleY = posY + drawH / 2.0f;
            float radius = drawW / 2.0f + 10;
            DrawCircleV({circleX, circleY}, radius, {170, 20, 20, 255});
        }

        DrawTextureEx(tex, {posX, posY}, 0.0f, scale, WHITE);
    }
}

void LocationInfoScene::drawVillagers()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    const int columns = 3;
    const int rows = 3;
    const int maxItems = columns * rows;
    float imageSize = 250.0f;
    float paddingX = -100.0f;
    float paddingY = -50.0f;
    float startX = 580.0f;
    float startY = 130.0f;

    for (int i = 0; i < villagerTextures.size() && i < maxItems; ++i)
    {
        Texture2D &tex = villagerTextures[i];
        float scale = imageSize / tex.width;
        float drawW = tex.width * scale;
        float drawH = tex.height * scale;
        int row = i / columns;
        int col = i % columns;
        float posX = startX + col * (imageSize + paddingX);
        float posY = startY + row * (imageSize + paddingY);
        DrawTextureEx(tex, {posX, posY}, 0.0f, scale, WHITE);
    }
}

void LocationInfoScene::drawItems()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    const int maxCols = 3;
    const int maxRows = 4;
    const int maxItems = maxCols * maxRows;

    float imageSize = 150.0f;
    float padding = 0.0f;

    float startX = 1100.0f;
    float startY = 200.0f;

    int drawCount = std::min((int)itemTextures.size(), maxItems);

    for (int i = 0; i < drawCount; ++i)
    {
        int row = i / maxCols;
        int col = i % maxCols;

        Texture2D &tex = itemTextures[i].tex;

        float scale = std::min(imageSize / tex.width, imageSize / tex.height);
        float drawW = tex.width * scale;
        float drawH = tex.height * scale;
        float posX = startX + col * (imageSize + padding) + (imageSize - drawW) / 2;
        float posY = startY + row * (imageSize + padding) + (imageSize - drawH) / 2;
        DrawTextureEx(tex, {posX, posY}, 0.0f, scale, WHITE);
    }
}

void LocationInfoScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    Rectangle src = {0, 0, (float)background.width, (float)background.height};
    Rectangle dest = {0, 0, (float)screenW, (float)screenH};
    Vector2 origin = {0, 0};

    DrawTexturePro(background, src, dest, origin, 0.0f, WHITE);

    drawCoffin();
    drawHeroAndMonster();
    drawVillagers();
    drawItems();

    ui.render();

    EndDrawing();
}

void LocationInfoScene::serialize(const std::string &filename)
{
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open())
        return;

    if (!outFile.is_open())
        return;

    outFile << "SceneKey:LocationInfoScene\n";
    outFile << "SceneData:\n";

    if (currentLocation)
        outFile << "LocationName:" << currentLocation->get_name() << "\n";
    else
        outFile << "LocationName:None\n";

    outFile.close();
}

void LocationInfoScene::deserialize(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        return;

    std::string line;
    bool isLocationInfoScene = false;
    bool inSceneData = false;

    while (std::getline(inFile, line))
    {
        if (!isLocationInfoScene)
        {
            if (line == "SceneKey:LocationInfoScene")
            {
                isLocationInfoScene = true;
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

        if (key == "LocationName")
        {
            currentLocation = nullptr;
            if (value != "None")
            {
                for (auto *loc : Map::get_instanse()->getAllLocations())
                {
                    if (loc->get_name() == value)
                    {
                        currentLocation = loc;
                        break;
                    }
                }
            }
        }
    }

    inFile.close();
}