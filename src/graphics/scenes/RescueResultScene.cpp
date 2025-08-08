#include "graphics/scenes/RescueResultScene.hpp"
#include "graphics/scenes/BoardScene.hpp"
#include "core/Game.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UILabel.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"

void RescueResultScene::onEnter()
{
    currentHero = Game::getInstance().getCurrentHero();
    rescuedVillager = Game::getInstance().getLastRescuedVillager();
    perkCard = Game::getInstance().getLastRewardedPerkCard();

    AudioManager::getInstance().stopMusic();
    AudioManager::getInstance().playRescueMusic();

    loadtextures();
    createUI();
}

void RescueResultScene::loadtextures()
{
    background = TextureManager::getInstance().getOrLoadTexture("rescue_bg", "assets/images/background/rescue_bg.png");

    std::string villagerPath = "assets/images/Villager/" + rescuedVillager->getName() + ".png";
    villagerTex = TextureManager::getInstance().getOrLoadTexture(rescuedVillager->getName(), villagerPath);

    std::string perkPath = "assets/images/Perk_Cards/" + perkCard.getName() + ".png";
    perkTex = TextureManager::getInstance().getOrLoadTexture(perkCard.getName(), perkPath);

    font = LoadFontEx("assets/fonts/simple.ttf", 40, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void RescueResultScene::createUI()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    Color titleColor = {255, 230, 180, 255};

    Vector2 messagePos = {screenW / 2.0f, 80};
    auto messageLabel = std::make_unique<UILabel>(
        messagePos, "Villager Rescued!", 44, 0.0f, WHITE, WHITE, true);

    messageLabel->setFont(font);
    messageLabel->enableBackground({0, 0, 0, 180}, 20.0f);
    uiManager.add(std::move(messageLabel));

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
                            SaveManager::getInstance().saveGameToSlot(SceneKeys::RESCUE_RESULT_SCENE); });

    uiManager.add(std::move(saveBtn));
}

void RescueResultScene::onExit()
{
    AudioManager::getInstance().stopRescueMusic();
    AudioManager::getInstance().playMusic();
    UnloadFont(font);
    uiManager.clear();

    elapsedTime = 0.0f;
    villagerAlpha = 0.0f;
    villagerFadeDone = false;
    perkY = -500.0f;
    perkSlideStarted = false;
    perkSlideDone = false;
}

void RescueResultScene::update(float deltaTime)
{
    elapsedTime += deltaTime;

    if (!villagerFadeDone)
    {
        villagerAlpha = elapsedTime / fadeDuration;
        if (villagerAlpha >= 1.0f)
        {
            villagerAlpha = 1.0f;
            villagerFadeDone = true;
            perkSlideStarted = true;
        }
    }

    if (perkSlideStarted && !perkSlideDone)
    {
        perkY += slideSpeed * deltaTime;
        if (perkY >= perkTargetY)
        {
            perkY = perkTargetY;
            perkSlideDone = true;
            AudioManager::getInstance().playSoundEffect("whoosh");
        }
    }

    AudioManager::getInstance().update();
    uiManager.update();
}

void RescueResultScene::render()
{
    ClearBackground(BLACK);
    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0, 0}, 0.0f, scaleX, WHITE);

    uiManager.render();

    if (rescuedVillager)
    {
        Color villagerColor = WHITE;
        villagerColor.a = static_cast<unsigned char>(villagerAlpha * 255);
        float imageMaxWidth = 600.0f;
        float imageScale = imageMaxWidth / villagerTex.width;

        float scaledWidth = villagerTex.width * imageScale;
        float scaledHeight = villagerTex.height * imageScale;

        Vector2 pos = {-100.0f, -100.0f};
        Rectangle src = {0, 0, (float)villagerTex.width, (float)villagerTex.height};
        Rectangle dst = {pos.x, pos.y, scaledWidth, scaledHeight};

        DrawTexturePro(villagerTex, src, dst, {0, 0}, 0.0f, villagerColor);
    }

    if (perkSlideStarted)
    {
        int cardWidth = 400;
        int cardHeight = 450;
        float scaleX = (float)cardWidth / perkTex.width;
        float scaleY = (float)cardHeight / perkTex.height;

        float scale = std::min(scaleX, scaleY);
        float drawW = perkTex.width * scale;
        float drawH = perkTex.height * scale;
        Vector2 pos = {GetScreenWidth() - drawW - 270.0f, perkY};
        Rectangle src = {0, 0, (float)perkTex.width, (float)perkTex.height};
        Rectangle dest = {pos.x, pos.y, drawW, drawH};
        DrawTexturePro(perkTex, src, dest, {0, 0}, 0.0f, WHITE);
    }
}