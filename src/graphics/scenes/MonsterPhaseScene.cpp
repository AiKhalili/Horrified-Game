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
    batSoundPlayed = false;

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
                                    int fontSize, float time, Font font, bool immediate, bool center, Color color)
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

// Handles the monster's movement and dice rolling logic during its turn.
void MonsterPhaseScene::step_MoveAndRoll(float deleteTime)
{
    handleMoveAndRoll(deleteTime); // Execute movement and dice roll logic

    // If all dice have been shown and results are ready
    if (!rolledDiceResult.empty() && diceShown >= (int)rolledDiceResult.size())
    {
        currentStep = MonsterPhaseStep::HandleStrike; // Move to strike phase
        processingStrike = false;
    }
}

// Checks if the monster phase should be skipped due to active Perk Card effects
void MonsterPhaseScene::step_CheckMonsterPhasePerk(float deltaTime)
{
    if (stepTimer == 0.0f)
    { // Determine if monster phase should be skipped
        skipPhase = !game.canStartMonsterPhase();

        if (skipPhase)
        {
            Vector2 center = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
            std::string msg = "Monster Phase skipped due to Perk Card!";
            showMessage(msg, center, 40, 2.5f, normalFont, false, true, RED);

            game.setGameState(GameState::EndMonsterPhase); // Update game state
        }
    }

    stepTimer += deltaTime; // Increment step timer

    if (skipPhase && stepTimer >= 3.0f)
    { // If skipping, wait then return to board scene
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    }

    if (!skipPhase)
    { // If not skipping, proceed to draw monster card
        currentStep = MonsterPhaseStep::DrawMonsterCard;
        stepTimer = 0.0f;
    }
}

// Handles the animation and logic for drawing the monster card.
void MonsterPhaseScene::step_DrawMonsterCard(float deltaTime)
{
    if (!cardDrawn)
    { // If the card hasn't been drawn yet, initialize it
        game.drawMonsterCard();
        card = game.currentMosnterCard;
        strikes = game.MonstersStrike;

        if (!game.targetMonster)
        {
            std::cerr << "ERROR: setupMonsterStrike failed! currentStrikeIndex="
                      << game.currentStrikeIndex
                      << " strikeName=" << game.MonstersStrike[game.currentStrikeIndex] << std::endl;
        }

        game.useMonsterCard(card.get_name()); // Apply the card's effect

        std::string name = card.get_name();
        std::string path = "assets/images/Monster_Cards/" + name + ".png";
        cardTexture = TextureManager::getInstance().getOrLoadTexture(name, path);

        AudioManager::getInstance().playSoundEffect("whoosh");

        // Initialize animation parameters
        cardScale = 0.0f;
        cardDrawn = true;
        stepTimer = 0.0f;
    }

    // Update animation timer
    stepTimer += deltaTime;
    float animDuration = 1.0f;
    float t = stepTimer / animDuration;
    if (t > 1.0f)
        t = 1.0f;

    // Scale the card (zoom in effect)
    cardScale = t;

    // Animate card sliding from bottom to corner
    float cardW = 300.0f * cardScale; // Start at bottom center
    float cardH = 400.0f * cardScale; // End at top-right corner

    Vector2 startPos = {800 - cardW / 2, 900};
    Vector2 targetPos = {1600 - cardW - 50, 50};

    cardPos.x = startPos.x + (targetPos.x - startPos.x) * t;
    cardPos.y = startPos.y + (targetPos.y - startPos.y) * t;

    if (t >= 1.0f && stepTimer >= animDuration + 1.0f)
    { // After animation is complete and small delay, move to next step
        currentStep = MonsterPhaseStep::PlaceItems;
        stepTimer = 0.0f;
    }
}

// Handles the placement and display of items during the Monster Phase.
void MonsterPhaseScene::step_PlaceItems(float deltaTime)
{
    if (!itemsPlaced)
    {
        items = game.placeItemsOnMap(); // Retrieve the items to place on the map
        itemsPlaced = true;

        // Initialize animation-related variables
        itemsTimer = 0.0f;
        itemsAlpha = 0.0f;
        showItemIcons = true;

        if (!items.empty())
        {
            AudioManager::getInstance().playSoundEffect("magic");
        }

        std::string msg;

        if (items.empty())
        { // Message if no items were placed
            msg = "No items were placed this round!";
        }
        else
        { // Message showing how many items were placed
            msg = std::to_string(items.size()) + " items have been placed on the map!";
        }

        Vector2 center = {500, 600}; // Position of the message

        showMessage(msg, center, 35, 5.0f, normalFont, true);

        for (auto *item : items)
        { // Load textures for each item to display
            std::string texPath = "assets/images/Items/" + item->get_color_to_string() + "/" + item->get_name() + ".png";
            itemTex.push_back(TextureManager::getInstance().getOrLoadTexture(item->get_name(), texPath));
        }
    }

    // Update timer and fade in alpha for items
    itemsTimer += deltaTime;
    if (itemsAlpha < 1.0f)
    {
        itemsAlpha += deltaTime * 0.2f;
    }

    if (itemsTimer >= 5.0f)
    { // After 5 seconds, hide icons and move to next step
        showItemIcons = false;
        stepTimer = 0.0f;
        currentStep = MonsterPhaseStep::RunEvent;
    }
}

// Advances the event phase by accumulating time.
void MonsterPhaseScene::step_RunEvent(float deleteTime)
{
    stepTimer += deleteTime;
    if (stepTimer >= 6.0f)
    { // Reset flags related to event message display.
        titelAdded = false;
        messageShown = false;
        batSoundPlayed = false;
        stepTimer = 0.0f;
        game.currentStrikeIndex = 0;
        currentStep = MonsterPhaseStep::MonsterMoveAndRoll;
    }
}

void MonsterPhaseScene::render_FormOfTheBat(float deleteTime)
{
    if (!batSoundPlayed)
    { // Play bat flying sound only once at animation start
        AudioManager::getInstance().setSoundVolume(10.0f);
        AudioManager::getInstance().playSoundEffect("flying");
        AudioManager::getInstance().setSoundVolume(0.5f);
        batSoundPlayed = true;
    }

    // Normalize time (0 to 1) over 6 seconds
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    // Calculate fade-out effect as animation progresses
    float fadeFactor = 1.0f - powf(t, 1.2f);
    if (fadeFactor < 0.0f)
    {
        fadeFactor = 0.0f;
    }

    // Define start and end positions for bat movement
    Vector2 startPos = {50, 900};
    Vector2 endPos = {800, 0};

    // Interpolate bat position over time
    Vector2 basePos = {
        startPos.x + (endPos.x - startPos.x) * t,
        startPos.y + (endPos.y - startPos.y) * t};

    // Apply vertical sinusoidal oscillation for natural flight
    float oscillationAmplitude = 20.0f;
    float frequency = 1.0f;
    float yOffset = sinf(deleteTime * frequency * 2 * PI) * oscillationAmplitude * (1.0f - t);
    basePos.y += yOffset;

    // Gradually scale down the bat image during movement
    float scale = 2.0f - t * 1.2f;

    // Set color with alpha for fade-out effect
    Color c = WHITE;
    c.a = (unsigned char)(255 * fadeFactor);

    DrawTextureEx(bats, basePos, 0.0f, scale, c);

    if (!titelAdded)
    { // Add title label once at the beginning
        Color color = {235, 235, 235, 255};
        Vector2 pos = {480, 100};
        auto title = std::make_unique<UILabel>(pos, "Form of the Bat", 100, 6.0f, color, color);
        title->setFont(spookyFont);
        ui.add(std::move(title));
        titelAdded = true;
    }

    if (deleteTime >= 3.0f && !messageShown)
    { // Show event message at halfway point (after 3 seconds)
        showMessage(game.event.msg, {500, 600}, 35, 3.0f, normalFont, true);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_Thief(float deleteTime)
{
    const float flashDuration = 1.0f; // Duration of the white flash effect at the start (1 second)

    // Show white flash overlay if within flash duration
    if (deleteTime < flashDuration)
    {
        float alphaFactor = 1.0f - (deleteTime / flashDuration);                // Fade out over time
        unsigned char a = static_cast<unsigned char>(255 * alphaFactor * 0.6f); // Max alpha capped at 60%
        DrawRectangle(0, 0, 1600, 900, Color{255, 255, 255, a});                // Full-screen white flash with fading alpha
    }

    if (!titelAdded)
    { // Add title label only once
        Color color = {235, 235, 235, 255};
        Vector2 pos = {650, 100};
        auto title = std::make_unique<UILabel>(pos, "Thief", 100, 6.0f, color, color);
        title->setFont(spookyFont);
        ui.add(std::move(title));
        titelAdded = true; // Prevent adding again
    }
    std::string msg = game.event.msg;

    if (deleteTime >= 2.0f && !messageShown)
    { // Show the message after 2 seconds, only once
        showMessage(msg, {530, 600}, 35, 4.0f, normalFont, true);
        messageShown = true; // Prevent message from showing again
    }
}

void MonsterPhaseScene::render_Sunrise(float deleteTime)
{
    const float flashDuration = 0.6f; // Duration of initial bright flash
    const float glowDuration = 1.5f;  // Duration of soft glowing effect afterward

    if (deleteTime < flashDuration)
    {                                                                           // Bright flash at the beginning (warm orange-white color)
        float alphaFactor = 1.0f - (deleteTime / flashDuration);                // Linearly decrease opacity
        unsigned char a = static_cast<unsigned char>(255 * alphaFactor * 0.6f); // Fade out to 60% max
        DrawRectangle(0, 0, 1600, 900, Color{255, 220, 150, a});
    }

    if (deleteTime >= flashDuration && deleteTime < flashDuration + glowDuration)
    {                                                              // Glowing sunrise rings and ambient light overlay
        float glowT = (deleteTime - flashDuration) / glowDuration; // Normalize time [0,1]
        if (glowT > 1.0f)
        {
            glowT = 1.0f;
        }

        float overlayAlphaF = sinf(glowT * PI);                                  // Smooth glow using sine wave
        unsigned char overlayA = static_cast<unsigned char>(overlayAlphaF * 80); // Soft orange ambient light
        DrawRectangle(0, 0, 1600, 900, Color{255, 200, 100, overlayA});

        Vector2 moonPos = {710, 335}; // Center of the glow rings

        float radiusBase = 150.0f + 150.0f * glowT; // Expanding radius over time
        float intensity = sinf(glowT * PI);         // Smooth intensity

        int rings = 70; // Number of glow rings
        for (int i = 0; i < rings; ++i)
        {
            float factor = 1.0f - (float)i / (float)rings; // Inner rings more intense
            float r = radiusBase * factor;
            float aFactor = intensity * factor * 0.2f;
            if (aFactor > 1.0f)
            {
                aFactor = 1.0f;
            }
            unsigned char a = static_cast<unsigned char>(255 * aFactor); // Rings opacity

            DrawCircleV(moonPos, r, Color{255, 223, 128, a});
        }
    }

    if (!titelAdded)
    { // Add title once at the top
        Color color = {235, 235, 235, 255};
        Vector2 pos = {600, 100};
        auto title = std::make_unique<UILabel>(pos, "Sunrise", 100, 6.0f, color, color);
        title->setFont(spookyFont);
        ui.add(std::move(title));
        titelAdded = true;
    }

    if (deleteTime >= 0.4f && !messageShown)
    { // Show event message after short delay
        showMessage(game.event.msg, {430, 600}, 35, 5.6f, normalFont, true);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_villagerCard(float deleteTime)
{
    // Normalize time to [0, 1] over 6 seconds
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    // Determine fade-in factor (only fades in during the first 0.2 seconds)
    float fade = (t < 0.2f) ? t / 0.2f : 1.0f;

    if (!loadedVillager)
    { // Load the villager texture only once
        std::string path = "assets/images/Villager/" + game.event.villagerName + ".png";
        villagerTex = TextureManager::getInstance().getOrLoadTexture(game.event.villagerName, path);
        loadedVillager = true;
    }

    // Calculate scaling factor to keep image within max width (600px)
    float imageMaxWidth = 600.0f;
    float imageScale = imageMaxWidth / villagerTex.width;

    // Apply the scale to determine final image size
    float scaledWidth = villagerTex.width * imageScale;
    float scaledHeight = villagerTex.height * imageScale;

    // Position to draw
    Vector2 imagePos = {-100, -100};
    Rectangle src = {0, 0, (float)villagerTex.width, (float)villagerTex.height};
    Rectangle dst = {imagePos.x, imagePos.y, scaledWidth, scaledHeight};

    DrawTexturePro(villagerTex, src, dst, {0, 0}, 0.0f,
                   Color{255, 255, 255, (unsigned char)(fade * 255)});

    if (!titelAdded)
    { // Add the card title (only once)
        Color color = {235, 235, 235, 255};
        Vector2 pos = {750, 100};
        auto titleLabel = std::make_unique<UILabel>(pos, card.get_name(), 100, 6.0f, color, color, true);
        titleLabel->setFont(spookyFont);
        ui.add(std::move(titleLabel));
        titelAdded = true;
    }

    if (deleteTime >= 2.0f && !messageShown)
    { // Show the event message after a short delay (2 seconds)
        std::string msg = game.event.msg;
        showMessage(msg, {750, 600}, 35, 4.0f, normalFont, true, true);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_HypnoticGaze(float deleteTime)
{
    // Normalize time to range [0, 1] over 6 seconds
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    // Compute fade-in alpha for the first 0.2 seconds
    float fade = (t < 0.2f) ? t / 0.2f : 1.0f;

    // Determine image position and max width based on whether a hero or villager is shown
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
    { // Load the correct texture once based on whether it's a hero or villager
        std::string name = game.event.villagerName.empty() ? game.event.heroName : game.event.villagerName;
        std::string folder = game.event.villagerName.empty() ? "heros" : "Villager";
        std::string path = "assets/images/" + folder + "/" + name + ".png";
        texHypnoticGaze = TextureManager::getInstance().getOrLoadTexture(name, path);
        loadedHypnotic = true;
    }

    // Compute final scaled width and height
    float scale = imageMaxWidth / texHypnoticGaze.width;
    float scaledW = texHypnoticGaze.width * scale;
    float scaledH = texHypnoticGaze.height * scale;

    // Prepare source and destination rectangles for drawing
    Rectangle src = {0, 0, (float)texHypnoticGaze.width, (float)texHypnoticGaze.height};
    Rectangle dst = {imagePos.x, imagePos.y, scaledW, scaledH};

    DrawTexturePro(texHypnoticGaze, src, dst, {0, 0}, 0.0f, {255, 255, 255, (unsigned char)(fade * 255)});

    if (!titelAdded)
    { // Add the title label only once
        Color color = {235, 235, 235, 255};
        Vector2 pos = {520, 100};
        std::string title = "Hypnotic Gaze";
        auto titleLabel = std::make_unique<UILabel>(pos, title, 100, 6.0f, color, color);
        titleLabel->setFont(spookyFont);
        ui.add(std::move(titleLabel));
        titelAdded = true;
    }

    if (deleteTime >= 2.0f && !messageShown)
    { // Show the message text after 2 seconds
        showMessage(game.event.msg, {450, 600}, 35, 4.0f, normalFont, true);
        messageShown = true;
    }
}

void MonsterPhaseScene::render_OnTheMove(float deleteTime)
{
    // Normalize the deleteTime to a 0.0 - 1.0 range for animation timing
    float t = deleteTime / 6.0f;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    // Calculate the fade-in value for smooth appearance
    float fade = (t < 0.2f) ? t / 0.2f : 1.0f;

    // Early phase: show the frenzy background and monster name
    if (deleteTime < 2.5f)
    {
        float fade = (t < 0.2f) ? t / 0.2f : 1.0f;

        Rectangle src = {0, 0, (float)frenzy.width, (float)frenzy.height};
        Rectangle dst = {0, 0, 1600.0, 900.0};

        // Draw the full-screen frenzy texture with transparency
        Color overlayColor = {255, 255, 255, (unsigned char)(fade * 200)};

        DrawTexturePro(frenzy, src, dst, {0, 0}, 0.0f, overlayColor);

        // Center and display monster name text
        std::string text = game.event.monsterName;
        Vector2 textSize = MeasureTextEx(spookyFont, text.c_str(), 80, 2.0f);
        Vector2 position = {(1600.0f - textSize.x) / 2.0f - 60, 400};

        DrawTextEx(spookyFont, text.c_str(), position, 80, 2, BLACK);
        return;
    }

    if (!titelAdded && deleteTime >= 2.5f)
    { // Add title label once, after the initial phase
        Color color = {235, 235, 235, 255};
        Vector2 pos = {550, 100};
        auto titleLabel = std::make_unique<UILabel>(pos, "On The Move", 80, 3.5f, color, color);
        titleLabel->setFont(spookyFont);
        ui.add(std::move(titleLabel));
        titelAdded = true;
    }

    if (!villagerImagesLoaded)
    { // Load villager images if not already loaded
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

    // Villager movement animation parameters
    float delayPerVillager = 0.8f;
    float totalTravelTime = 3.5f;
    float startX = -100.0f;
    float endX = 1600 + 100.0f;
    float y = 600;

    // Animate villagers with delay and smooth movement
    for (size_t i = 0; i < villagerNames.size(); ++i)
    {
        float startDelay = i * delayPerVillager;
        float t = deleteTime - 2.5f - startDelay;

        if (t < 0.0f || t > totalTravelTime)
        { // Skip if it's not yet their time to appear or already done
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
    { // Show the summary message after animation starts
        std::string msg = "Frenzy marker moved to " + game.event.monsterName + " and villagers moved 1 step toward safety.";
        showMessage(msg, {330, 600}, 30, 3.0f, normalFont, true);
        messageShown = true;
    }
}

void MonsterPhaseScene::renderDice(float deltaTime)
{
    if (rolledDiceResult.empty())
    { // Exit early if there are no dice results to show
        return;
    }

    diceTimer += deltaTime; // Accumulate time to control dice reveal timing

    if (diceShown < (int)rolledDiceResult.size() && diceTimer >= 1.5f)
    { // Show next dice result every 1.5 seconds
        diceShown++;
        diceTimer = 0.0f;
    }

    // Positioning parameters for dice rendering
    float startX = 50;
    float startY = 500;
    float spacingY = 120;

    for (int i = 0; i < diceShown; ++i)
    { // Loop through shown dice and draw them one by one
        const std::string &result = rolledDiceResult[i];
        Texture2D *tex = nullptr;

        // Choose the correct texture based on dice result
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
        { // If texture exists, render it
            float alpha = 1.0f;
            if (i == diceShown - 1 && diceShown != (int)rolledDiceResult.size())
            { // Apply fade-in effect for the most recently revealed dice
                alpha = std::min(diceTimer / 1.0f, 1.0f);
            }

            Color c = WHITE;
            c.a = (unsigned char)(alpha * 255);

            float size = 100.0f;
            float scale = size / tex->width;
            Vector2 pos = {startX, startY + i * spacingY};

            DrawTextureEx(*tex, pos, 0.0f, scale, c);

            // Draw the text label next to the dice
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
    game.setupMonsterStrike();
    // if (!game.targetMonster)
    // {
    //     std::cerr << "[INFO] targetMonster is NULL — calling setupMonsterStrike()\n";
    //     game.setupMonsterStrike();
    //     processingStrike = false;
    // }

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