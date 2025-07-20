#include "graphics/scenes/LoadGamesScene.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "core/Game.hpp"
#include "saves/SaveManager.hpp"

void LoadGamesScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("LoadGame", "assets/images/background/showLoad.png");
    normalFont = LoadFont("assets/fonts/simple.ttf");
    spooky = LoadFont("assets/fonts/spooky.otf");
    createButtons();
}

void LoadGamesScene::onExit()
{
    UnloadFont(normalFont);
    ui.clear();
}

void LoadGamesScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    ui.update();
}

void LoadGamesScene::render()
{
    ClearBackground(BLACK);
    DrawTexture(background, 0, 0, WHITE);

    const char *title = "Select a saved game to continue ...";
    Vector2 textSize = MeasureTextEx(spooky, title, 36, 2);

    float x = (1600 - textSize.x) / 2;
    float y = 40;

    DrawTextEx(spooky, title, {x, y}, 36, 2, WHITE);
    ui.render();
}

void LoadGamesScene::createButtons()
{
    SaveManager save;
    auto slots = save.getAllSaveSlotsWithTimestamps();

    float btnWidth = 280;
    float btnHeight = 80;
    float startX = 150;
    float startY = 120;
    float gapX = 300;
    float gapY = 130;

    int maxPerRow = 5;

    for (size_t i = 0; i < slots.size(); ++i)
    {
        int row = i / maxPerRow;
        int col = i % maxPerRow;

        float x = startX + col * gapX;
        float y = startY + row * gapY;

        Rectangle bounds = {x, y, btnWidth, btnHeight};
        std::string label = "Slot " + std::to_string(slots[i].first) + "\n" + slots[i].second;

        auto button = std::make_unique<UIButton>(bounds, label, 22, WHITE, DARKGRAY, GRAY);
        button->setFont(normalFont);

        int slotNumber = slots[i].first;

        button->setOnClick([slotNumber, &save]()
                           {
            AudioManager::getInstance().playSoundEffect("click");
            save.loadGameFromSlot(slotNumber, Game::getInstance());
            SceneManager::getInstance().goTo("BoardScene"); });

        ui.add(std::move(button));
    }

    Rectangle backBounds = {1600 - 300, 900 - 100, 240, 60};
    auto backBtn = std::make_unique<UIButton>(backBounds, "Main Menu", 26, WHITE, DARKGRAY, GRAY);
    backBtn->setFont(normalFont);
    backBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo("MainMenuScene"); });

    ui.add(std::move(backBtn));
}