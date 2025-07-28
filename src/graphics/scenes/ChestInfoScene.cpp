#include "graphics/scenes/ChestInfoScene.hpp"
#include "core/Game.hpp"
#include "graphics/scenes/BoardScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UILabel.hpp"
#include "audio/AudioManager.hpp"

void ChestInfoScene::onEnter()
{
    currentHero = Game::getInstance().getCurrentHero();
    background = LoadTexture("assets/images/background/wooden.png");
    font = LoadFontEx("assets/fonts/simple.ttf", 70, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    AudioManager::getInstance().playMusic();
    createUI();

    loadHeroItems();
}

void ChestInfoScene::createUI()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    Color titleColor = GOLD;
    Color textColor = {240, 220, 190, 255};

    Vector2 titlePos = {(screenW / 2.0f) - 120, 60};

    auto titleLabel = std::make_unique<UILabel>(
        titlePos, "Hero's Items", 70, 0.0f, titleColor, titleColor);
    titleLabel->setFont(font);
    uiManager.add(std::move(titleLabel));

    if (currentHero && currentHero->getItems().empty())
    {
        Vector2 centerPos = {(screenW / 2.0f) - 205, screenH / 2.0f};

        auto noItemsLabel = std::make_unique<UILabel>(
            centerPos, "Your inventory is empty.", 45, 0.0f, textColor, textColor);

        noItemsLabel->enableBackground({50, 30, 20, 200}, 20.0f);
        noItemsLabel->setFont(font);
        uiManager.add(std::move(noItemsLabel));
    }

    Rectangle backBounds = {screenW / 2.0f - 120, screenH - 90, 240, 60};

    auto backBtn = std::make_unique<UIButton>(
        backBounds, "Back", 28, WHITE,
        Color{101, 67, 33, 255}, Color{50, 30, 20, 200}, Color{100, 50, 10, 255});

    backBtn->setFont(font);
    backBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });
    uiManager.add(std::move(backBtn));
}

void ChestInfoScene::onExit()
{
    UnloadFont(font);
    uiManager.clear();
    itemTextures.clear();
}

void ChestInfoScene::loadHeroItems()
{
    auto &items = currentHero->getItems();

    for (auto *item : items)
    {
        std::string colorFolder = item->get_color_to_string();
        std::string path = "assets/images/Items/" + colorFolder + "/" + item->get_name() + ".png";
        Texture2D tex = TextureManager::getInstance().getOrLoadTexture(item->get_name(), path);
        itemTextures.push_back({tex, item->get_color()});
    }

    std::sort(itemTextures.begin(), itemTextures.end(), [](const HeroItemTex &a, const HeroItemTex &b)
              { return (int)a.color < (int)b.color; });
}

void ChestInfoScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    uiManager.update();
}

void ChestInfoScene::render()
{
    ClearBackground(BLACK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    Rectangle src = {0, 0, (float)background.width, (float)background.height};
    Rectangle dest = {0, 0, (float)screenW, (float)screenH};
    Vector2 origin = {0, 0};

    DrawTexturePro(background, src, dest, origin, 0.0f, WHITE);

    int startX = 200;
    int startY = 150;
    int gapX = 100;
    int gapY = 160;
    int maxPerRow = 10;

    int targetSize = 120;

    int x = startX;
    int y = startY;
    int count = 0;

    for (auto &heroItem : itemTextures)
    {
        Texture2D &tex = heroItem.tex;

        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dest = {(float)x, (float)y, (float)targetSize, (float)targetSize};
        Vector2 origin = {0, 0};

        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);

        x += gapX;
        count++;

        if (count >= maxPerRow)
        {
            count = 0;
            x = startX;
            y += gapY;
        }
    }

    uiManager.render();
}