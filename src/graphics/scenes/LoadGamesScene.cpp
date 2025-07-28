#include "graphics/scenes/LoadGamesScene.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "core/Game.hpp"
#include "saves/SaveManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/scenes/SceneKeys.hpp"

void LoadGamesScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("LoadGame", "assets/images/background/showLoad.png");
    normalFont = LoadFont("assets/fonts/simple.ttf");
    SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);
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

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);

    if (!SaveManager::getInstance().getAllSaveSlotsWithTimestamps().empty())
    {
        const char *title = "Select a saved game to continue ...";
        Vector2 textSize = MeasureTextEx(normalFont, title, 44, 2);

        float x = (1600 - textSize.x) / 2;
        float y = 40;

        DrawTextEx(normalFont, title, {x, y}, 44, 2, WHITE);
    }
    ui.render();
}

void LoadGamesScene::createButtons()
{
    auto slots = SaveManager::getInstance().getAllSaveSlotsWithTimestamps();

    float btnWidth = 280;
    float btnHeight = 80;
    float startX = 80;
    float startY = 150;
    float gapX = 300;
    float gapY = 130;

    int maxPerRow = 5;

    Color buttonColor = {255, 230, 180, 255};

    if (slots.empty())
    {
        Vector2 center = {600, 100};
        Color color = {240, 220, 190, 160};
        auto label = std::make_unique<UILabel>(center, "No Saved Games Found!", 44, 0.0f, color, color);
        label->enableBackground(BLACK, 20.0f);
        label->setFont(normalFont);
        ui.add(std::move(label));
    }
    else
    {
        for (size_t i = 0; i < slots.size(); ++i)
        {
            int row = i / maxPerRow;
            int col = i % maxPerRow;

            float x = startX + col * gapX;
            float y = startY + row * gapY;

            Rectangle bounds = {x, y, btnWidth, btnHeight};
            std::string label = "Slot " + std::to_string(slots[i].first) + "\n" + slots[i].second;

            auto button = std::make_unique<UIButton>(bounds, label, 22, buttonColor, BLACK, DARKGRAY, buttonColor);
            button->setFont(normalFont);

            int slotNumber = slots[i].first;

            button->setOnClick([slotNumber]()
                               {
            AudioManager::getInstance().playSoundEffect("click");
            Game::getInstance().startLoadedGame(slotNumber);
            SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });

            ui.add(std::move(button));
        }
    }

    Rectangle backBounds = {1600 - 300, 900 - 100, 240, 60};
    auto backBtn = std::make_unique<UIButton>(backBounds, "Main Menu", 26, buttonColor, BLACK, DARKGRAY, buttonColor);
    backBtn->setFont(normalFont);
    backBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });

    ui.add(std::move(backBtn));
}