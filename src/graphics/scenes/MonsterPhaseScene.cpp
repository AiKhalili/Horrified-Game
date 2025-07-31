#include "graphics/scenes/MonsterPhaseScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "core/Game.hpp"
#include "raymath.h"

void MonsterPhaseScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("monster_phase", "assets/images/background/monster_phase.png");
    bats = TextureManager::getInstance().getOrLoadTexture("bats", "assets/images/icon/bats.png");

    normalFont = LoadFont("assets/fonts/simple.ttf");
    spookyFont = LoadFontEx("assets/fonts/spooky.otf", 100, 0, 0);

    SetTextureFilter(spookyFont.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);

    AudioManager::getInstance().playMusic();
}

void MonsterPhaseScene::onExit()
{
    stepTimer = 0.0f;
    titelAdded = false;
    messageShown = false;
    UnloadFont(normalFont);
    UnloadFont(spookyFont);
    ui.clear();
}

void MonsterPhaseScene::renderBackGround()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0.0}, 0.0f, scaleX, WHITE);
}

void MonsterPhaseScene::renderMonsterCard()
{
    if (cardDrawn)
    {
        float cardW = 300.0f * cardScale;
        float cardH = 400.0f * cardScale;

        Rectangle src = {0, 0, (float)cardTexture.width, (float)cardTexture.height};
        Rectangle dst = {cardPos.x, cardPos.y, cardW, cardH};
        Vector2 origin = {0, 0};

        DrawTexturePro(cardTexture, src, dst, origin, 0.0f, WHITE);
    }
}

void MonsterPhaseScene::showMessage(const std::string &text, Vector2 pos, int fontSize, float time)
{
    Color color = {235, 235, 235, 255};
    auto label = std::make_unique<UILabel>(pos, text, fontSize, time, color, color);
    label->enableBackground(BLACK, 20.0f);
    label->setFont(normalFont);
    ui.add(std::move(label));
}

void MonsterPhaseScene::renderItems()
{
    if (showItemIcons && !itemTex.empty())
    {
        float startX = 80; // موقعیت شروع
        float y = 700;     // پایین صفحه
        float spacing = 150;

        for (size_t i = 0; i < itemTex.size(); ++i)
        {
            float size = 120; // اندازه آیکون
            Rectangle src = {0, 0, (float)itemTex[i].width, (float)itemTex[i].height};
            Rectangle dst = {startX + i * spacing, y, size, size};
            Vector2 origin = {0, 0};

            Color fadeColor = WHITE;
            fadeColor.a = (unsigned char)(itemsAlpha * 255);
            DrawTexturePro(itemTex[i], src, dst, origin, 0.0f, fadeColor);
        }
    }
}

void MonsterPhaseScene::render()
{
    renderBackGround();
    renderMonsterCard();

    switch (currentStep)
    {
    case MonsterPhaseStep::PlaceItems:
        renderItems();
        break;
    case MonsterPhaseStep::RunEvent:
        renderEvents();
        break;
    }

    ui.render();
}

void MonsterPhaseScene::renderEvents()
{
    if (card.get_name() == "FormOfTheBat")
    {
        render_FormOfTheBat(stepTimer);
    }
    else if (card.get_name() == "Thief")
    {
        render_Thief(stepTimer);
    }
    else if (card.get_name() == "Sunrise")
    {
        render_Sunrise(stepTimer);
    }
    else if (card.get_name() == "TheDelivery")
    {
        render_villagerCard(stepTimer);
    }
}

void MonsterPhaseScene::update(float deleteTime)
{
    switch (currentStep)
    {
    case MonsterPhaseStep::CheckMonsterPhasePerk:
        step_CheckMonsterPhasePerk(deleteTime);
        break;
    case MonsterPhaseStep::DrawMonsterCard:
        step_DrawMonsterCard(deleteTime);
        break;
    case MonsterPhaseStep::PlaceItems:
        step_PlaceItems(deleteTime);
        break;
    case MonsterPhaseStep::RunEvent:
        step_RunEvent(deleteTime);
        break;
    }

    ui.update();
}

void MonsterPhaseScene::step_CheckMonsterPhasePerk(float deltaTime)
{
    if (stepTimer == 0.0f)
    {
        skipPhase = !game.canStartMonsterPhase();

        if (skipPhase)
        {
            Vector2 center = {600, 100};
            std::string msg = "Monster Phase skipped due to Perk Card!";

            game.setGameState(GameState::EndMonsterPhase);
        }
    }

    stepTimer += deltaTime;

    if (skipPhase && stepTimer >= 3.0f)
    {
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    }

    if (!skipPhase)
    {
        currentStep = MonsterPhaseStep::DrawMonsterCard;
        stepTimer = 0.0f;
    }
}

void MonsterPhaseScene::step_DrawMonsterCard(float deltaTime)
{
    if (!cardDrawn)
    {
        game.drawMonsterCard();
        card = game.currentMosnterCard;
        game.useMonsterCard(card.get_name());

        std::string name = card.get_name();
        std::string path = "assets/images/Monster_Cards/" + name + ".png";
        cardTexture = TextureManager::getInstance().getOrLoadTexture(name, path);

        cardScale = 0.0f;
        cardDrawn = true;
        stepTimer = 0.0f;
    }

    // انیمیشن کارت
    stepTimer += deltaTime;
    float animDuration = 1.0f;
    float t = stepTimer / animDuration;
    if (t > 1.0f)
        t = 1.0f;

    // Scale
    cardScale = t;

    // Slide
    float cardW = 300.0f * cardScale;
    float cardH = 400.0f * cardScale;

    Vector2 startPos = {800 - cardW / 2, 900};   // پایین وسط
    Vector2 targetPos = {1600 - cardW - 50, 50}; // گوشه راست بالا

    cardPos.x = startPos.x + (targetPos.x - startPos.x) * t;
    cardPos.y = startPos.y + (targetPos.y - startPos.y) * t;

    // بعد از انیمیشن برو مرحله بعد
    if (t >= 1.0f && stepTimer >= animDuration + 1.0f)
    {
        currentStep = MonsterPhaseStep::PlaceItems;
        stepTimer = 0.0f;
    }
}

void MonsterPhaseScene::step_PlaceItems(float deltaTime)
{
    //  فقط یکبار آیتم‌ها رو قرار بده
    if (!itemsPlaced)
    {
        items = game.placeItemsOnMap(); // از Game آیتم‌ها رو بگیر
        itemsPlaced = true;
        itemsTimer = 0.0f;
        itemsAlpha = 0.0f;
        showItemIcons = true;

        // // 🎵 صدای قرار گرفتن آیتم‌ها
        // AudioManager::getInstance().playSFX("place_items");

        std::string msg;

        if (items.empty())
        {
            msg = "No items were placed this round!";
        }
        else
        {
            msg = std::to_string(items.size()) + " items have been placed on the map!";
        }

        Vector2 center = {500, 600};

        showMessage(msg, center, 35, 5.0f);

        for (auto *item : items)
        {
            std::string texPath = "assets/images/Items/" + item->get_color_to_string() + "/" + item->get_name() + ".png";
            itemTex.push_back(TextureManager::getInstance().getOrLoadTexture(item->get_name(), texPath));
        }
    }

    itemsTimer += deltaTime;
    if (itemsAlpha < 1.0f)
    {
        itemsAlpha += deltaTime * 0.2f;
    }

    if (itemsTimer >= 5.0f)
    {
        showItemIcons = false;
        stepTimer = 0.0f;
        currentStep = MonsterPhaseStep::RunEvent;
    }
}

void MonsterPhaseScene::step_RunEvent(float deleteTime)
{
    stepTimer += deleteTime;
    if (stepTimer >= 6.0f)
    {
        titelAdded = false;
        messageShown = false;
        stepTimer = 0.0f;
        currentStep = MonsterPhaseStep::Move;
    }
}

void MonsterPhaseScene::render_FormOfTheBat(float deleteTime)
{
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    float fadeFactor = 1.0f - powf(t, 1.2f);
    if (fadeFactor < 0.0f)
    {
        fadeFactor = 0.0f;
    }

    // موقعیت اولیه و نهایی
    Vector2 startPos = {50, 900};
    Vector2 endPos = {800, 0};

    // موقعیت پایه بین دو نقطه
    Vector2 basePos = {
        startPos.x + (endPos.x - startPos.x) * t,
        startPos.y + (endPos.y - startPos.y) * t};

    float oscillationAmplitude = 20.0f;

    float frequency = 1.0f;

    float yOffset = sinf(deleteTime * frequency * 2 * PI) * oscillationAmplitude * (1.0f - t);

    basePos.y += yOffset;

    // تغییر مقیاس (از ۲ به حدود ۰.۸)
    float scale = 2.0f - t * 1.2f;

    // محو شدن تدریجی
    Color c = WHITE;
    c.a = (unsigned char)(255 * fadeFactor);

    // رسم خفاش
    DrawTextureEx(bats, basePos, 0.0f, scale, c);

    // نوشتن عنوان کارت
    if (!titelAdded)
    {
        Color color = {235, 235, 235, 255};
        Vector2 pos = {500, 100};
        auto title = std::make_unique<UILabel>(pos, "Form of the Bat", 100, 6.0f, color, color);
        title->setFont(spookyFont);
        ui.add(std::move(title));
        titelAdded = true;
    }

    // پیام پایانی
    if (deleteTime >= 4.0f && !messageShown)
    {
        showMessage(game.event.msg, {550, 600}, 35, 3.0f);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_Thief(float deleteTime)
{
    const float flashDuration = 1.0f;

    if (deleteTime < flashDuration)
    {
        float alphaFactor = 1.0f - (deleteTime / flashDuration);
        unsigned char a = static_cast<unsigned char>(255 * alphaFactor * 0.6f);
        DrawRectangle(0, 0, 1600, 900, Color{255, 255, 255, a});
    }
    // نوشتن عنوان کارت
    if (!titelAdded)
    {
        Color color = {235, 235, 235, 255};
        Vector2 pos = {700, 100};
        auto title = std::make_unique<UILabel>(pos, "Thief", 100, 6.0f, color, color);
        title->setFont(spookyFont);
        ui.add(std::move(title));
        titelAdded = true;
    }
    std::string msg = game.event.msg;

    if (deleteTime >= 2.0f && !messageShown)
    {
        showMessage(msg, {550, 600}, 35, 4.0f);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_Sunrise(float deleteTime)
{
    const float flashDuration = 0.6f;
    const float glowDuration = 1.5f;

    if (deleteTime < flashDuration)
    {
        float alphaFactor = 1.0f - (deleteTime / flashDuration);
        unsigned char a = static_cast<unsigned char>(255 * alphaFactor * 0.6f);
        DrawRectangle(0, 0, 1600, 900, Color{255, 220, 150, a});
    }

    if (deleteTime >= flashDuration && deleteTime < flashDuration + glowDuration)
    {
        float glowT = (deleteTime - flashDuration) / glowDuration;
        if (glowT > 1.0f)
        {
            glowT = 1.0f;
        }

        float overlayAlphaF = sinf(glowT * PI);
        unsigned char overlayA = static_cast<unsigned char>(overlayAlphaF * 80);
        DrawRectangle(0, 0, 1600, 900, Color{255, 200, 100, overlayA});

        Vector2 moonPos = {710, 335};

        float radiusBase = 150.0f + 150.0f * glowT;
        float intensity = sinf(glowT * PI);

        int rings = 70;
        for (int i = 0; i < rings; ++i)
        {
            float factor = 1.0f - (float)i / (float)rings;
            float r = radiusBase * factor;
            float aFactor = intensity * factor * 0.2f;
            if (aFactor > 1.0f)
            {
                aFactor = 1.0f;
            }
            unsigned char a = static_cast<unsigned char>(255 * aFactor);

            DrawCircleV(moonPos, r, Color{255, 223, 128, a});
        }
    }

    if (!titelAdded)
    {
        Color color = {235, 235, 235, 255};
        Vector2 pos = {600, 100};
        auto title = std::make_unique<UILabel>(pos, "Sunrise", 100, 6.0f, color, color);
        title->setFont(spookyFont);
        ui.add(std::move(title));
        titelAdded = true;
    }

    if (deleteTime >= 0.4f && !messageShown)
    {
        showMessage(game.event.msg, {450, 600}, 35, 5.6f);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_villagerCard(float deleteTime)
{
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    float fade = (t < 0.4f) ? t / 0.4f : 1.0f;

    static bool loaded = false;
    if (!loaded)
    {
        std::string path = "assets/images/Villager/" + game.event.villagerName + ".png";
        villagerTex = TextureManager::getInstance().getOrLoadTexture(game.event.villagerName, path);
        loaded = true;
    }
    float imageMaxWidth = 600.0f;
    float imageScale = imageMaxWidth / villagerTex.width;

    float scaledWidth = villagerTex.width * imageScale;
    float scaledHeight = villagerTex.height * imageScale;

    Vector2 imagePos = {-100, -100};
    Rectangle src = {0, 0, (float)villagerTex.width, (float)villagerTex.height};
    Rectangle dst = {imagePos.x, imagePos.y, scaledWidth, scaledHeight};

    DrawTexturePro(villagerTex, src, dst, {0, 0}, 0.0f,
                   Color{255, 255, 255, (unsigned char)(fade * 255)});

    if (!titelAdded)
    {
        Color color = {235, 235, 235, 255};
        Vector2 pos = {550, 100};
        auto titleLabel = std::make_unique<UILabel>(pos, card.get_name(), 100, 6.0f, color, color);
        titleLabel->setFont(spookyFont);
        ui.add(std::move(titleLabel));
        titelAdded = true;
    }

    if (deleteTime >= 2.0f && !messageShown)
    {
        std::string msg = game.event.msg;
        showMessage(msg, {450, 600}, 35, 4.0f);
        messageShown = true;
    }
}