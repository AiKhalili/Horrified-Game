#include "graphics/scenes/MonsterPhaseScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "core/Game.hpp"
#include "graphics/scenes/ItemSelectionScene.hpp"
#include "core/SceneDataHub.hpp"
#include "raymath.h"

void MonsterPhaseScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("monster_phase", "assets/images/background/monster_phase.png");
    bats = TextureManager::getInstance().getOrLoadTexture("bats", "assets/images/icon/bats.png");
    frenzy = TextureManager::getInstance().getOrLoadTexture("hex_frenzy", "assets/images/background/hex_frenzy.jpg");

    diceBlank = TextureManager::getInstance().getOrLoadTexture("diceBlank", "assets/images/icon/diceBlank.png");
    diceStrike = TextureManager::getInstance().getOrLoadTexture("diceStrike", "assets/images/icon/diceStrike.png");
    dicePower = TextureManager::getInstance().getOrLoadTexture("dicePower", "assets/images/icon/dicePower.png");

    normalFont = LoadFont("assets/fonts/simple.ttf");
    spookyFont = LoadFontEx("assets/fonts/spooky.otf", 100, 0, 0);

    SetTextureFilter(spookyFont.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);
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

void MonsterPhaseScene::showMessage(const std::string &text, Vector2 pos, int fontSize, float time, Font font, Color color)
{
    auto label = std::make_unique<UILabel>(pos, text, fontSize, time, color, color);
    label->enableBackground(BLACK, 20.0f);
    label->setFont(font);
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
    case MonsterPhaseStep::MonsterMoveAndRoll:
        if (game.targetMonster)
        {
            renderCurrentMonster();
        }
        break;
    case MonsterPhaseStep::HandleStrike:
        renderCurrentMonster();
        break;
    case MonsterPhaseStep::HandlePower:
        renderCurrentMonster();
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
    else if (card.get_name() == "FortuneTeller")
    {
        render_villagerCard(stepTimer);
    }
    else if (card.get_name() == "FormerEmployer")
    {
        render_villagerCard(stepTimer);
    }
    else if (card.get_name() == "HurriedAssistant")
    {
        render_villagerCard(stepTimer);
    }
    else if (card.get_name() == "TheInnocent")
    {
        render_villagerCard(stepTimer);
    }
    else if (card.get_name() == "EgyptionExpert")
    {
        render_villagerCard(stepTimer);
    }
    else if (card.get_name() == "TheIchthyologist")
    {
        render_villagerCard(stepTimer);
    }
    else if (card.get_name() == "HypnoticGaze")
    {
        render_HypnoticGaze(stepTimer);
    }
    else if (card.get_name() == "OnTheMove")
    {
        render_OnTheMove(stepTimer);
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
    case MonsterPhaseStep::MonsterMoveAndRoll:
        step_MoveAndRoll(deleteTime);
        break;
    case MonsterPhaseStep::HandleStrike:
        step_HandleStrike(deleteTime);
        break;
    }

    ui.update();
}

void MonsterPhaseScene::step_MoveAndRoll(float deleteTime)
{
    handleMoveAndRoll(deleteTime);
    if (!rolledDiceResult.empty() && diceShown >= (int)rolledDiceResult.size())
    {
        currentStep = MonsterPhaseStep::HandleStrike;
        processingStrike = false;
    }
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
        strikes = game.MonstersStrike;
        // game.setupMonsterStrike();
        if (!game.targetMonster)
        {
            std::cerr << "ERROR: setupMonsterStrike failed! currentStrikeIndex="
                      << game.currentStrikeIndex
                      << " strikeName=" << game.MonstersStrike[game.currentStrikeIndex] << std::endl;
        }
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

        showMessage(msg, center, 35, 5.0f, normalFont);

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
        game.currentStrikeIndex = 0;
        currentStep = MonsterPhaseStep::MonsterMoveAndRoll;
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
        showMessage(game.event.msg, {550, 600}, 35, 3.0f, normalFont);
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
        Vector2 pos = {650, 100};
        auto title = std::make_unique<UILabel>(pos, "Thief", 100, 6.0f, color, color);
        title->setFont(spookyFont);
        ui.add(std::move(title));
        titelAdded = true;
    }
    std::string msg = game.event.msg;

    if (deleteTime >= 2.0f && !messageShown)
    {
        showMessage(msg, {550, 600}, 35, 4.0f, normalFont);
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
        showMessage(game.event.msg, {450, 600}, 35, 5.6f, normalFont);
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

    float fade = (t < 0.2f) ? t / 0.2f : 1.0f;

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
        Vector2 pos = {530, 100};
        auto titleLabel = std::make_unique<UILabel>(pos, card.get_name(), 100, 6.0f, color, color);
        titleLabel->setFont(spookyFont);
        ui.add(std::move(titleLabel));
        titelAdded = true;
    }

    if (deleteTime >= 2.0f && !messageShown)
    {
        std::string msg = game.event.msg;
        showMessage(msg, {500, 600}, 35, 4.0f, normalFont);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_HypnoticGaze(float deleteTime)
{
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    float fade = (t < 0.2f) ? t / 0.2f : 1.0f;
    Vector2 imagePos;
    float imageMaxWidth;

    if (game.event.heroName.empty())
    {
        imagePos = {-100, -100};
        imageMaxWidth = 600.0f;
    }
    else
    {
        imagePos = {0, 0};
        imageMaxWidth = 350.0f;
    }

    static Texture2D tex = {};
    static bool loaded = false;
    if (!loaded)
    {
        std::string name = game.event.villagerName.empty() ? game.event.heroName : game.event.villagerName;
        std::string folder = game.event.villagerName.empty() ? "heros" : "Villager";
        std::string path = "assets/images/" + folder + "/" + name + ".png";
        tex = TextureManager::getInstance().getOrLoadTexture(name, path);
        loaded = true;
    }

    float scale = imageMaxWidth / tex.width;
    float scaledW = tex.width * scale;
    float scaledH = tex.height * scale;

    Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
    Rectangle dst = {imagePos.x, imagePos.y, scaledW, scaledH};

    DrawTexturePro(tex, src, dst, {0, 0}, 0.0f, {255, 255, 255, (unsigned char)(fade * 255)});

    if (!titelAdded)
    {
        Color color = {235, 235, 235, 255};
        Vector2 pos = {550, 100};
        std::string title = "Hypnotic Gaze";
        auto titleLabel = std::make_unique<UILabel>(pos, title, 100, 6.0f, color, color);
        titleLabel->setFont(spookyFont);
        ui.add(std::move(titleLabel));
        titelAdded = true;
    }

    if (deleteTime >= 2.0f && !messageShown)
    {
        showMessage(game.event.msg, {450, 600}, 35, 4.0f, normalFont);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_OnTheMove(float deleteTime)
{
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    float fade = (t < 0.2f) ? t / 0.2f : 1.0f;

    if (deleteTime < 2.5f)
    {
        float fade = (t < 0.2f) ? t / 0.2f : 1.0f;

        Rectangle src = {0, 0, (float)frenzy.width, (float)frenzy.height};
        Rectangle dst = {0, 0, 1600.0, 900.0};

        Color overlayColor = {255, 255, 255, (unsigned char)(fade * 200)};

        DrawTexturePro(frenzy, src, dst, {0, 0}, 0.0f, overlayColor);

        std::string text = game.event.monsterName;
        DrawTextEx(spookyFont, text.c_str(), {560, 400}, 80, 2, BLACK);
        return;
    }

    if (!titelAdded && deleteTime >= 2.5f)
    {
        Color color = {235, 235, 235, 255};
        Vector2 pos = {550, 100};
        auto titleLabel = std::make_unique<UILabel>(pos, "On The Move", 80, 3.5f, color, color);
        titleLabel->setFont(spookyFont);
        ui.add(std::move(titleLabel));
        titelAdded = true;
    }

    static std::vector<std::string> villagerNames;
    static std::vector<Texture2D> villagerTextures;
    static bool villagerImagesLoaded = false;

    if (!villagerImagesLoaded)
    {
        villagerNames.clear();
        villagerTextures.clear();

        for (auto &pair : game.event.villagersAndLocations)
        {
            std::string name = pair.first;
            villagerNames.push_back(name);
            std::string path = "assets/images/Villager/" + name + ".png";
            Texture2D tex = TextureManager::getInstance().getOrLoadTexture(name, path);
            villagerTextures.push_back(tex);
        }
        villagerImagesLoaded = true;
    }

    float delayPerVillager = 0.4f;
    float totalTravelTime = 3.5f;
    float startX = -100.0f;
    float endX = 1600 + 100.0f;
    float y = 900 - 120.0f;

    for (size_t i = 0; i < villagerNames.size(); ++i)
    {
        float startDelay = i * delayPerVillager;
        float t = deleteTime - 2.5f - startDelay;
        if (t < 0.0f || t > totalTravelTime)
            continue;

        float progress = t / totalTravelTime;
        float x = startX + (endX - startX) * progress;
        Texture2D tex = villagerTextures[i];
        float scale = 60.0f / tex.height;
        float w = tex.width * scale;
        float h = tex.height * scale;
        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dst = {x, y, w, h};
        DrawTexturePro(tex, src, dst, {0, 0}, 0.0f, WHITE);
    }

    if (deleteTime >= 3.0f && !messageShown)
    {
        std::string msg = "Frenzy marker moved to " + game.event.monsterName + " and villagers moved 1 step toward safety.";
        showMessage(msg, {330, 600}, 30, 3.0f, normalFont);
        messageShown = true;
    }
}

void MonsterPhaseScene::renderDice(float deltaTime)
{
    if (rolledDiceResult.empty())
    {
        return;
    }

    diceTimer += deltaTime;

    if (diceShown < (int)rolledDiceResult.size() && diceTimer >= 1.5f)
    {
        diceShown++;
        diceTimer = 0.0f;
    }

    float startX = 50;
    float startY = 500;
    float spacingY = 120;

    for (int i = 0; i < diceShown; ++i)
    {
        const std::string &result = rolledDiceResult[i];
        Texture2D *tex = nullptr;

        if (result == "Blank")
        {
            tex = &diceBlank;
        }
        else if (result == "Strike")
        {
            tex = &diceStrike;
        }
        else if (result == "Power")
        {
            tex = &dicePower;
        }

        if (tex)
        {
            float alpha = 1.0f;
            if (i == diceShown - 1 && diceShown != (int)rolledDiceResult.size())
            {
                alpha = std::min(diceTimer / 1.0f, 1.0f);
            }

            Color c = WHITE;
            c.a = (unsigned char)(alpha * 255);

            float size = 100.0f;
            float scale = size / tex->width;
            Vector2 pos = {startX, startY + i * spacingY};

            DrawTextureEx(*tex, pos, 0.0f, scale, c);

            DrawTextEx(normalFont, result.c_str(), {startX + 120, startY + i * spacingY + 30}, 30, 2, WHITE);
        }
    }
}

void MonsterPhaseScene::executeStrike()
{
    StrikeResult result = game.diceStrikeFace();

    if (result == StrikeResult::HeroFound)
    {
        std::string msg = game.targetMonster->get_name() + " attacks a " + game.damageHero->getClassName() + "!";
        showMessage(msg, {500, 600}, 40, 3.0f, spookyFont);

        std::vector<Item *> heroItems = game.damageHero->getItems();

        if (heroItems.empty())
        {
            showMessage("Hero has no defense items!", {500, 600}, 35, 3.0f, normalFont);
            game.sendHeroToHospital();
            remainingStrikes--;
            processingStrike = false;
            return;
        }

        SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE).setData(heroItems, SceneKeys::MONSTER_PHASE_SCENE);

        SceneManager::getInstance().goTo(SceneKeys::ITEM_SELECTION_SCENE);

        return;
    }
    else if (result == StrikeResult::VillagerFound)
    {
        std::string msg = "Villager " + game.damageVillager->getName() + " was killed!";
        showMessage(msg, {500, 600}, 40, 3.0f, spookyFont);
        remainingStrikes = 0;
        currentStep = MonsterPhaseStep::EndPhase;
        processingStrike = false;
        return;
    }

    remainingStrikes--;
    processingStrike = false;
}

void MonsterPhaseScene::renderCurrentMonster()
{
    if (!game.targetMonster)
    {
        std::cout << "target monster is NUL\n";
        return;
    }

    Monster *m = game.targetMonster;
    Texture2D tex = TextureManager::getInstance().getOrLoadTexture(
        m->get_name(), "assets/images/Monsters/" + m->get_name() + ".png");

    Vector2 pos = {50, 50};
    float imageSize = 270.0f;
    float scale = imageSize / tex.width;
    DrawTextureEx(tex, pos, 0.0f, scale, WHITE);

    std::string location = "LOCATION : " + m->get_location()->get_name();
    showMessage(location, {30, 400}, 35, 0.0f, normalFont, {170, 20, 20, 255});

    renderDice(GetFrameTime());
}

void MonsterPhaseScene::handleMoveAndRoll(float deleteTime)
{
    if (!game.targetMonster)
    {
        std::cerr << "[INFO] targetMonster is NULL — calling setupMonsterStrike()\n";
        game.setupMonsterStrike();
        processingStrike = false;
    }

    if (!game.targetMonster)
    {
        std::cerr << "[FATAL] targetMonster is STILL NULL after setup — aborting strike\n";
        return;
    }

    if (!processingStrike)
    {
        processingStrike = true;

        if (!game.targetMonster)
        {
            std::cerr << "ERROR: targetMonster is NULL before moveMonster()" << std::endl;
        }
        bool moved = game.moveMonster();

        game.diceCount.clear();
        rolledDiceResult = game.rollingDice();
        diceTimer = 0.0f;
        diceShown = 0;

        remainingStrikes = game.diceCount[Face::STRIKE];

        std::cout << "[DEBUG] Remaining Strikes after dice roll: " << remainingStrikes << std::endl;

        if (moved)
        {
            std::string msg = game.targetMonster->get_name() + " moved to " +
                              game.targetMonster->get_location()->get_name() + "!";
            showMessage(msg, {550, 600}, 35, 3.0f, normalFont);
        }
        else
        {
            std::string msg = game.targetMonster->get_name() + " stayed at " +
                              game.targetMonster->get_location()->get_name() + ".";
            showMessage(msg, {550, 600}, 35, 3.0f, normalFont);
        }
    }
}

void MonsterPhaseScene::step_HandleStrike(float deleteTime)
{
    auto &selectedItems = SceneDataHub::getInstance().getSelectedItems();

    if (!selectedItems.empty())
    {
        handleDefence(selectedItems);
        SceneDataHub::getInstance().reset();
        return;
    }

    if (remainingStrikes > 0 && !processingStrike)
    {
        processingStrike = true;
        executeStrike();
    }
    else if (remainingStrikes <= 0 && currentStep != MonsterPhaseStep::EndPhase)
    {
        currentStep = MonsterPhaseStep::HandlePower;
        processingStrike = false;
    }
}

void MonsterPhaseScene::handleDefence(std::vector<Item *> &selectedItems)
{
    if (selectedItems.empty())
    {
        showMessage("No defense item used!", {500, 600}, 35, 3.0f, normalFont);
        game.sendHeroToHospital();
    }
    else if (selectedItems.size() > 1)
    {
        showMessage("Only ONE item allowd for defense!", {500, 600}, 35, 3.0f, normalFont);

        SceneManager::getInstance().goTo(SceneKeys::ITEM_SELECTION_SCENE);
        return;
    }
    else
    {
        Item *defItem = selectedItems[0];

        std::string text = "Hero defends using " + defItem->get_name() + "!";
        showMessage(text, {500, 600}, 35, 3.0f, normalFont);
        game.defendHero(defItem);
    }

    processingStrike = false;
}