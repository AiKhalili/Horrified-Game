#include "graphics/scenes/MonsterPhaseScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "core/Game.hpp"
#include "graphics/scenes/ItemSelectionScene.hpp"
#include "core/SceneDataHub.hpp"
#include "raymath.h"

// called when entering the monster phase scene
void MonsterPhaseScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("monster_phase", "assets/images/background/monster_phase.png");
    bats = TextureManager::getInstance().getOrLoadTexture("bats", "assets/images/icon/bats.png");
    frenzy = TextureManager::getInstance().getOrLoadTexture("hex_frenzy", "assets/images/background/hex_frenzy.jpg");

    diceBlank = TextureManager::getInstance().getOrLoadTexture("diceBlank", "assets/images/icon/diceBlank.png");
    diceStrike = TextureManager::getInstance().getOrLoadTexture("diceStrike", "assets/images/icon/diceStrike.png");
    dicePower = TextureManager::getInstance().getOrLoadTexture("dicePower", "assets/images/icon/dicePower.png");

    bloodOverLay = TextureManager::getInstance().getOrLoadTexture("bloodOverLay", "assets/images/background/blood.png");

    AudioManager::getInstance().stopMusic();
    AudioManager::getInstance().playMonsterSelectMusic();

    normalFont = LoadFont("assets/fonts/simple.ttf");
    spookyFont = LoadFontEx("assets/fonts/spooky.otf", 100, 0, 0);

    SetTextureFilter(spookyFont.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);
}

// called when exiting the monster phase scene
void MonsterPhaseScene::onExit()
{
    stepTimer = 0.0f;
    diceTimer = 0.0f;
    diceShown = 0;
    remainingStrikes = 0;
    messageTimer = 0.0f;
    itemsAlpha = 0.0f;
    itemsTimer = 0.0f;
    cardScale = 0.0f;
    bloodOverlayTimer = 0.0f;

    titelAdded = false;
    messageShown = false;
    processingStrike = false;
    waitingForMessage = false;
    showItemIcons = false;
    itemsPlaced = false;
    cardDrawn = false;
    showBloodOverlay = false;
    powerVillagerKilled = false;
    villagerImagesLoaded = false;
    loadedVillager = false;
    loadedHypnotic = false;
    isWaitingForDefenceSelection = false;

    powerTargetHero = nullptr;
    powerTargetVillager = nullptr;
    locationLabel = nullptr;

    while (!messageQueue.empty())
    {
        messageQueue.pop();
    }
    while (!messageDuration.empty())
    {
        messageDuration.pop();
    }

    rolledDiceResult.clear();
    villagerNames.clear();
    villagerTextures.clear();
    items.clear();
    itemTex.clear();

    currentStep = MonsterPhaseStep::CheckMonsterPhasePerk;

    AudioManager::getInstance().stopMonsterSelectMusic();
    AudioManager::getInstance().playMusic();

    UnloadFont(normalFont);
    UnloadFont(spookyFont);
    ui.clear();
}

// Clear the screen and draw the monster phase background scaled to fit width
void MonsterPhaseScene::renderBackGround()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0.0}, 0.0f, scaleX, WHITE);
}

// Draw the monster card with animation scale and position
void MonsterPhaseScene::renderMonsterCard()
{
    if (cardDrawn)
    {
        float cardW = 300.0f * cardScale;
        float cardH = 400.0f * cardScale;

        // Define source and destination rectangles for the texture
        Rectangle src = {0, 0, (float)cardTexture.width, (float)cardTexture.height};
        Rectangle dst = {cardPos.x, cardPos.y, cardW, cardH};
        Vector2 origin = {0, 0};

        DrawTexturePro(cardTexture, src, dst, origin, 0.0f, WHITE);
    }
}

// Create and enqueue a UI label to display a message on screen
void MonsterPhaseScene::showMessage(const std::string &text, Vector2 pos,
                                    int fontSize, float time, Font font, bool immediate, Color color, bool center)
{
    // Initialize the UILabel
    auto label = std::make_unique<UILabel>(pos, text, fontSize, time, color, color, center);
    label->enableBackground(BLACK, 20.0f);
    label->setFont(font);

    if (time <= 0.0f || immediate)
    { // If the message should show immediately or stay permanently, render it now
        ui.add(std::move(label));
        return;
    }

    // Otherwise, queue the message for timed display
    messageQueue.push(std::move(label));
    messageDuration.push(time);

    if (!waitingForMessage)
    { // If no message is currently being shown, display this one
        displayNextMessage();
    }
}

// Display the next message from the queue, or handle item selection if needed
void MonsterPhaseScene::displayNextMessage()
{
    if (messageQueue.empty())
    {
        waitingForMessage = false;

        // If a pending item selection is required, move to the item selection scene
        if (pendingItemSelection)
        {
            pendingItemSelection = false;
            isWaitingForDefenceSelection = true;

            SceneManager::getInstance().getScene<MonsterPhaseScene>(SceneKeys::MONSTER_PHASE_SCENE).setSkip(true);
            SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE).setData(pendingHeroItems, SceneKeys::MONSTER_PHASE_SCENE);
            SceneManager::getInstance().goTo(SceneKeys::ITEM_SELECTION_SCENE);
        }
        return;
    }

    // Pop the next message from the queue and display it
    auto label = std::move(messageQueue.front());
    messageQueue.pop();

    float duration = messageDuration.front();
    messageDuration.pop();

    messageTimer = duration;
    waitingForMessage = true;
    ui.add(std::move(label));
}

// Renders the item icons at the bottom of the screen if they are enabled and available
void MonsterPhaseScene::renderItems()
{
    if (showItemIcons && !itemTex.empty())
    {
        // set initial position ans spacing for item icons
        float startX = 80;
        float y = 700;
        float spacing = 150;

        for (size_t i = 0; i < itemTex.size(); ++i)
        {
            // Set size and source/destination rectangles for each item
            float size = 120;
            Rectangle src = {0, 0, (float)itemTex[i].width, (float)itemTex[i].height};
            Rectangle dst = {startX + i * spacing, y, size, size};
            Vector2 origin = {0, 0};

            // Apply fade effect using alpha transparency
            Color fadeColor = WHITE;
            fadeColor.a = (unsigned char)(itemsAlpha * 255);
            DrawTexturePro(itemTex[i], src, dst, origin, 0.0f, fadeColor);
        }
    }
}

// Main render function for MonsterPhaseScene.
// Responsible for drawing background, monster card, current step visuals, UI, and overlays.
void MonsterPhaseScene::render()
{
    renderBackGround();
    renderMonsterCard();

    if (game.targetMonster &&
        (currentStep == MonsterPhaseStep::MonsterMoveAndRoll ||
         currentStep == MonsterPhaseStep::HandleStrike ||
         currentStep == MonsterPhaseStep::HandlePower ||
         currentStep == MonsterPhaseStep::EndPhase))
    { // Render the current active monster during specific steps
        renderCurrentMonster();
    }

    switch (currentStep)
    { // Render additional visuals depending on the current game step
    case MonsterPhaseStep::PlaceItems:
        renderItems();
        break;
    case MonsterPhaseStep::RunEvent:
        renderEvents();
        break;
    case MonsterPhaseStep::HandlePower:
        renderPowerEffect();
        break;
    }

    ui.render();

    if (showBloodOverlay)
    { // If blood overlay is enabled, draw it with scaling
        DrawTextureEx(bloodOverLay, {0, 0}, 0.0f, (float)GetScreenHeight() / bloodOverLay.height, WHITE);
    }
}

// Render different event animations based on the current card's name.
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

// Update logic for the MonsterPhaseScene, called every frame.
void MonsterPhaseScene::update(float deleteTime)
{
    AudioManager::getInstance().update();
    ui.update();

    if (showBloodOverlay)
    { // Handle blood overlay fade-out effect
        bloodOverlayTimer -= deleteTime;
        if (bloodOverlayTimer <= 0.0f)
        {
            showBloodOverlay = false;
        }
    }

    if (waitingForMessage)
    { // If we're waiting for a message to finish displaying
        messageTimer -= deleteTime;
        if (messageTimer <= 0.0f)
        {
            displayNextMessage(); // Show next message
            if (!waitingForMessage && processingPowerQueue)
            {
                processNextPower();
            }
        }
        return;
    }

    if (delaySceneChange)
    { // Handle delayed scene change
        sceneChangeTimer -= deleteTime;
        if (sceneChangeTimer <= 0.0f)
        {
            delaySceneChange = false;
            SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
        }
        return;
    }

    // Main step-based state machine handling
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
    case MonsterPhaseStep::HandlePower:
        step_HandlePower(deleteTime);
        break;
    case MonsterPhaseStep::ManageStrike:
        handleManageStrike();
        break;
    case MonsterPhaseStep::EndPhase:
        endMonsterPhase();
        break;
    }
}

void MonsterPhaseScene::step_MoveAndRoll(float deleteTime)
{
    handleMoveAndRoll(deleteTime);

    std::cout << "[DEBUG] diceShown = " << diceShown
              << " | rolledDiceResult.size() = " << rolledDiceResult.size() << std::endl;

    if (!rolledDiceResult.empty() && diceShown >= (int)rolledDiceResult.size())
    {
        std::cout << "[DEBUG] Switching to HandleStrike step\n";

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

        showMessage(msg, center, 35, 5.0f, normalFont, true);

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
        showMessage(game.event.msg, {550, 600}, 35, 3.0f, normalFont, true);
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
        showMessage(msg, {550, 600}, 35, 4.0f, normalFont, true);
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
        showMessage(game.event.msg, {450, 600}, 35, 5.6f, normalFont, true);
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

    if (!loadedVillager)
    {
        std::string path = "assets/images/Villager/" + game.event.villagerName + ".png";
        villagerTex = TextureManager::getInstance().getOrLoadTexture(game.event.villagerName, path);
        loadedVillager = true;
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
        showMessage(msg, {500, 600}, 35, 4.0f, normalFont, true);
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

    if (!loadedHypnotic)
    {
        std::string name = game.event.villagerName.empty() ? game.event.heroName : game.event.villagerName;
        std::string folder = game.event.villagerName.empty() ? "heros" : "Villager";
        std::string path = "assets/images/" + folder + "/" + name + ".png";
        texHypnoticGaze = TextureManager::getInstance().getOrLoadTexture(name, path);
        loadedHypnotic = true;
    }

    float scale = imageMaxWidth / texHypnoticGaze.width;
    float scaledW = texHypnoticGaze.width * scale;
    float scaledH = texHypnoticGaze.height * scale;

    Rectangle src = {0, 0, (float)texHypnoticGaze.width, (float)texHypnoticGaze.height};
    Rectangle dst = {imagePos.x, imagePos.y, scaledW, scaledH};

    DrawTexturePro(texHypnoticGaze, src, dst, {0, 0}, 0.0f, {255, 255, 255, (unsigned char)(fade * 255)});

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
        showMessage(game.event.msg, {450, 600}, 35, 4.0f, normalFont, true);
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

    float delayPerVillager = 0.8f;
    float totalTravelTime = 3.5f;
    float startX = -100.0f;
    float endX = 1600 + 100.0f;
    float y = 600;

    for (size_t i = 0; i < villagerNames.size(); ++i)
    {
        float startDelay = i * delayPerVillager;
        float t = deleteTime - 2.5f - startDelay;
        if (t < 0.0f || t > totalTravelTime)
        {
            continue;
        }

        float progress = t / totalTravelTime;
        float x = startX + (endX - startX) * progress;
        Texture2D tex = villagerTextures[i];
        float scale = 400.0f / tex.height;
        float w = tex.width * scale;
        float h = tex.height * scale;
        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dst = {x, y, w, h};
        DrawTexturePro(tex, src, dst, {0, 0}, 0.0f, WHITE);
    }

    if (deleteTime >= 3.0f && !messageShown)
    {
        std::string msg = "Frenzy marker moved to " + game.event.monsterName + " and villagers moved 1 step toward safety.";
        showMessage(msg, {330, 600}, 30, 3.0f, normalFont, true);
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
        std::string msg = game.targetMonster->get_name() + " attacks the " + game.damageHero->getClassName() + "!";
        showMessage(msg, {500, 600}, 40, 3.0f, spookyFont, false);

        std::vector<Item *> heroItems = game.damageHero->getItems();

        if (heroItems.empty())
        {
            showBloodOverlay = true;
            bloodOverlayTimer = 5.0f;
            showMessage("Hero has no defense items!", {500, 600}, 35, 3.0f, normalFont, false);
            game.sendHeroToHospital();
            remainingStrikes = 0;
            currentStep = MonsterPhaseStep::EndPhase;
            processingStrike = false;
            return;
        }
        remainingStrikes--;
        SceneManager::getInstance().getScene<MonsterPhaseScene>(SceneKeys::MONSTER_PHASE_SCENE).setSkip(true);
        SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE).setData(heroItems, SceneKeys::MONSTER_PHASE_SCENE);
        isWaitingForDefenceSelection = true;
        SceneManager::getInstance().goTo(SceneKeys::ITEM_SELECTION_SCENE);
        processingStrike = false;
        return;
    }
    else if (result == StrikeResult::VillagerFound)
    {
        showBloodOverlay = true;
        bloodOverlayTimer = 3.0f;
        std::string msg = "Villager " + game.damageVillager->getName() + " was killed!";
        showMessage(msg, {500, 600}, 40, 3.0f, spookyFont, false);
        remainingStrikes = 0;
        currentStep = MonsterPhaseStep::EndPhase;
        processingStrike = false;
        return;
    }
    else
    {
        std::string msg = game.targetMonster->get_name() + " attacks but finds no one!";
        showMessage(msg, {500, 600}, 35, 3.0f, normalFont, false);
        remainingStrikes--;
        processingStrike = false;
    }
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

    if (!locationLabel)
    {
        locationLabel = new UILabel({30, 400}, location, 35, 0.0f, {170, 20, 20, 255}, {170, 20, 20, 255});
        locationLabel->enableBackground(BLACK, 20.0f);
        locationLabel->setFont(normalFont);
        ui.add(std::unique_ptr<UILabel>(locationLabel));
    }
    else
    {
        locationLabel->setText(location);
    }

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

        std::cout << "[DEBUG] Rolled Dice Faces: ";
        for (auto &faceStr : rolledDiceResult)
            std::cout << faceStr << " ";
        std::cout << "\n";
        std::cout << "[DEBUG] STRIKE Count from diceCount: " << game.diceCount[Face::STRIKE] << "\n";
        std::cout << "[DEBUG] Setting remainingStrikes = " << game.diceCount[Face::STRIKE] << "\n";

        diceTimer = 0.0f;
        diceShown = 0;

        std::cout << "[DEBUG] Dice rolled: ";
        for (auto f : rolledDiceResult)
            std::cout << f << " ";
        std::cout << "\n";

        std::cout << "[DEBUG] STRIKE count = " << game.diceCount[Face::STRIKE] << std::endl;

        remainingStrikes = game.diceCount[Face::STRIKE];

        std::cout << "[DEBUG] Remaining Strikes after dice roll: " << remainingStrikes << std::endl;

        if (moved)
        {
            std::string msg = game.targetMonster->get_name() + " moved to " +
                              game.targetMonster->get_location()->get_name() + "!";
            showMessage(msg, {550, 600}, 35, 3.0f, normalFont, false);
        }
        else
        {
            std::string msg = game.targetMonster->get_name() + " stayed at " +
                              game.targetMonster->get_location()->get_name() + ".";
            showMessage(msg, {550, 600}, 35, 3.0f, normalFont, false);
            waitingForMessage = true;
        }
    }
}

void MonsterPhaseScene::step_HandleStrike(float deleteTime)
{
    if (waitingForMessage)
    {
        return;
    }

    if (isWaitingForDefenceSelection)
    {
        auto &selectedItems = SceneDataHub::getInstance().getSelectedItems();
        handleDefence(selectedItems);
        SceneDataHub::getInstance().reset();
        // isWaitingForDefenceSelection = false;
        if (!pendingItemSelection)
        {
            isWaitingForDefenceSelection = false;
        }
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
        showMessage("No defense item used!", {500, 600}, 35, 3.0f, normalFont, false);
        game.sendHeroToHospital();
        SceneDataHub::getInstance().reset();
        showBloodOverlay = true;
        bloodOverlayTimer = 5.0f;
        remainingStrikes = 0;
        currentStep = MonsterPhaseStep::EndPhase;
        processingStrike = false;
        isWaitingForDefenceSelection = false;
        return;
    }
    else if (selectedItems.size() > 1)
    {
        std::vector<Item *> heroItems = game.damageHero->getItems();

        SceneDataHub::getInstance().reset();

        pendingHeroItems = heroItems;
        pendingItemSelection = true;

        showMessage("Only ONE item allowd for defense!", {500, 600}, 35, 3.0f, normalFont, false);
        return;
    }
    else
    {
        Item *defItem = selectedItems[0];

        std::string text = "Hero defends using " + defItem->get_name() + "!";
        showMessage(text, {500, 600}, 35, 3.0f, normalFont, false);
        game.defendHero(defItem);
        isWaitingForDefenceSelection = false;
    }

    SceneDataHub::getInstance().reset();
    processingStrike = false;
}

void MonsterPhaseScene::step_HandlePower(float delta)
{
    if (waitingForMessage || processingPowerQueue)
    {
        return;
    }

    int powerCount = game.diceCount[Face::POWER];

    if (powerCount > 0)
    {
        for (int i = 0; i < powerCount; ++i)
        {
            PowerResult result = game.targetMonster->specialPower(game.getCurrentHero());
            powerQueue.push(result);
        }
        processingPowerQueue = true;
        processNextPower();
    }
    else
    {
        currentStep = MonsterPhaseStep::ManageStrike;
    }
}

void MonsterPhaseScene::processNextPower()
{
    if (!powerQueue.empty())
    {
        PowerResult result = powerQueue.front();
        powerQueue.pop();

        powerTargetHero = result.targetHero;
        powerTargetVillager = result.targetVillager;
        powerVillagerKilled = result.villagerKilled;
        float msgDuration = result.villagerKilled ? 4.0f : 3.0f;

        if (result.villagerKilled)
        {
            showBloodOverlay = true;
            bloodOverlayTimer = 4.0f;
            showMessage("Blood spills! " + result.targetVillager->getName() + " was killed!",
                        {550, 800}, 40, msgDuration, spookyFont, false);
        }
        else
        {
            showMessage(game.targetMonster->get_name() + " uses their special power!",
                        {550, 800}, 40, msgDuration, spookyFont, false);
        }

        waitingForMessage = true;
    }
    else
    {
        processingPowerQueue = false;
        powerTargetHero = nullptr;
        powerTargetVillager = nullptr;
        powerVillagerKilled = false;
        showBloodOverlay = false;
        currentStep = MonsterPhaseStep::ManageStrike;
    }
}

void MonsterPhaseScene::renderPowerEffect()
{
    std::cout << "[DEBUG] renderPowerEffect CALLED\n";

    if (powerTargetHero)
    {
        std::string name = powerTargetHero->getClassName();
        std::string path = "assets/images/heroes/" + name + ".png";

        std::cout << "[DEBUG] Hero Target: " << name << " | Path: " << path << "\n";

        Texture2D heroTex = TextureManager::getInstance().getOrLoadTexture(name, path);
        float desiredSize = 350.0f;
        float scale = desiredSize / heroTex.width;

        Vector2 center = {
            1600.0f / 2.0f - (heroTex.width * scale) / 2.0f - 80,
            900.0f / 2.0f - (heroTex.height * scale) / 2.0f};

        DrawTextureEx(heroTex, center, 0.0f, scale, WHITE);
        DrawRectangleLinesEx(
            {center.x - 5, center.y - 5,
             (heroTex.width * scale) + 20, (heroTex.height * scale) + 20},
            8, RED);

        DrawTextEx(spookyFont, "The Monster's Special Power targets this Hero!",
                   {center.x - 150, center.y + (heroTex.height * scale) + 20}, 40, 2, RED);

        std::cout << "[DEBUG] Hero drawn with scale " << scale << "\n";
    }
    else
    {
        std::cout << "[DEBUG] No Hero target for power effect.\n";
    }

    if (powerTargetVillager)
    {
        std::string name = powerTargetVillager->getName();
        std::string path = "assets/images/Villager/" + name + ".png";

        std::cout << "[DEBUG] Villager Target: " << name << " | Path: " << path << "\n";

        Texture2D villagerTex = TextureManager::getInstance().getOrLoadTexture(name, path);
        float desiredSize = 600.0f;
        float scale = desiredSize / villagerTex.width;

        Vector2 center = {
            1600.0f / 2.0f - (villagerTex.width * scale) / 2.0f - 180,
            900.0f / 2.0f - (villagerTex.height * scale) / 2.0f - 100};

        DrawTextureEx(villagerTex, center, 0.0f, scale, WHITE);
        DrawRectangleLinesEx(
            {center.x - 5, center.y - 5,
             (villagerTex.width * scale) + 20, (villagerTex.height * scale) + 20},
            8, RED);

        DrawTextEx(spookyFont, "The Monster's Special Power targets this Villager!",
                   {center.x - 150, center.y + (villagerTex.height * scale) + 20}, 40, 2, RED);

        std::cout << "[DEBUG] Villager drawn with scale " << scale << "\n";
    }
    else
    {
        std::cout << "[DEBUG] No Villager target for power effect.\n";
    }
}

void MonsterPhaseScene::handleManageStrike()
{
    std::cout << "[DEBUG] Entered handleManageStrike\n";
    std::cout << "[DEBUG] currentStrikeIndex = " << game.currentStrikeIndex
              << " / total = " << game.MonstersStrike.size() << "\n";

    if (game.currentStrikeIndex < (int)game.MonstersStrike.size() - 1)
    {
        game.currentStrikeIndex++;
        std::cout << "[DEBUG] Advancing to next strike: index = " << game.currentStrikeIndex << "\n";

        game.setupMonsterStrike();

        // رفتن به مرحله MoveAndRoll برای استرایک بعدی
        currentStep = MonsterPhaseStep::MonsterMoveAndRoll;
        processingStrike = false;

        return;
    }
    else
    {
        std::cout << "[DEBUG] All strikes processed. Going to EndPhase.\n";
        currentStep = MonsterPhaseStep::EndPhase;
    }
}

void MonsterPhaseScene::endMonsterPhase()
{
    std::cout << "[DEBUG] Ending Monster Phase...\n";

    game.currentState = GameState::EndMonsterPhase;

    delaySceneChange = true;
    sceneChangeTimer = 1.0f;

    // SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
}