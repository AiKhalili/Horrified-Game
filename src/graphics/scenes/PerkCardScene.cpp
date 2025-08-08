#include "graphics/scenes/PerkCardScene.hpp"
#include "core/Game.hpp"
#include "graphics/scenes/BoardScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UILabel.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"

void PerkCardScene::onEnter()
{
    currentHero = Game::getInstance().getCurrentHero();
    background = TextureManager::getInstance().getOrLoadTexture("wooden", "assets/images/background/wooden.png");
    font = LoadFontEx("assets/fonts/simple.ttf", 70, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    AudioManager::getInstance().playMusic();
    createUI();

    loadPerkCardsTex();
}

void PerkCardScene::createUI()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    Color titleColor = GOLD;
    Color textColor = {240, 220, 190, 255};

    Vector2 titlePos = {(screenW / 2.0f) - 200, 40};

    auto titleLabel = std::make_unique<UILabel>(
        titlePos, "Hero's Perk Cards", 70, 0.0f, titleColor, titleColor);
    titleLabel->setFont(font);
    uiManager.add(std::move(titleLabel));

    if (currentHero && currentHero->getPerkCard().empty())
    {
        Vector2 centerPos = {(screenW / 2.0f) - 205, screenH / 2.0f};

        auto noItemsLabel = std::make_unique<UILabel>(
            centerPos, "Your inventory is empty.", 45, 0.0f, textColor, textColor);

        noItemsLabel->enableBackground({50, 30, 20, 200}, 20.0f);
        noItemsLabel->setFont(font);
        uiManager.add(std::move(noItemsLabel));
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
    uiManager.add(std::move(backBtn));

    Rectangle saveBounds = {btnX, screenH - btnHeight - btnMargin, btnWidth, btnHeight};

    auto saveBtn = std::make_unique<UIButton>(
        saveBounds, "Save", 28, WHITE,
        Color{101, 67, 33, 255},
        Color{50, 30, 20, 200},
        Color{100, 50, 10, 255});

    saveBtn->setFont(font);
    saveBtn->setOnClick([]()
                        {
                            AudioManager::getInstance().playSoundEffect("click");
                            SaveManager::getInstance().saveGameToSlot(SceneKeys::PERK_CARD_SCENE); });

    uiManager.add(std::move(saveBtn));
}

void PerkCardScene::onExit()
{
    UnloadFont(font);
    uiManager.clear();
    perkCardTextures.clear();
}

void PerkCardScene::loadPerkCardsTex()
{
    auto cards = currentHero->getPerkCard();

    for (auto card : cards)
    {
        std::string path = "assets/images/Perk_Cards/" + card.getName() + ".png";
        Texture2D tex = TextureManager::getInstance().getOrLoadTexture(card.getName(), path);
        perkCardTextures.push_back(tex);
    }
}

void PerkCardScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    uiManager.update();
}

void PerkCardScene::render()
{
    ClearBackground(BLACK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    Rectangle src = {0, 0, (float)background.width, (float)background.height};
    Rectangle dest = {0, 0, (float)screenW, (float)screenH};
    Vector2 origin = {0, 0};

    DrawTexturePro(background, src, dest, origin, 0.0f, WHITE);

    int cardWidth = 250;
    int cardHeight = 300;
    int gapX = 50;
    int gapY = 40;

    float totalWidth = (4 * cardWidth) + (3 * gapX);
    float startX = (screenW - totalWidth) / 2.0f;
    float startYTop = 150;
    float startYBottom = startYTop + cardHeight + gapY;

    for (size_t i = 0; i < perkCardTextures.size() && i < 8; i++)
    {
        Texture2D &tex = perkCardTextures[i];
        int row = i / 4;
        int col = i % 4;

        float x = startX + col * (cardWidth + gapX);
        float y = (row == 0) ? startYTop : startYBottom;

        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dest = {x, y, (float)cardWidth, (float)cardHeight};

        DrawTexturePro(tex, src, dest, {0, 0}, 0.0f, WHITE);
    }

    uiManager.render();
}