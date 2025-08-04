#ifndef MONSTERPHASESCENE_HPP
#define MONSTERPHASESCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"
#include <memory>
#include "core/Game.hpp"
#include <queue>

enum class MonsterPhaseStep
{
    None,
    CheckMonsterPhasePerk,
    DrawMonsterCard,
    PlaceItems,
    RunEvent,
    MonsterMoveAndRoll,
    HandleStrike,
    HandlePower,
    ManageStrike,
    EndPhase
};

class MonsterPhaseScene : public Scene
{
private:
    Texture2D background;
    Texture2D cardTexture;
    Texture2D bats;
    Texture2D villagerTex;
    Texture2D frenzy;
    Texture2D diceBlank;
    Texture2D diceStrike;
    Texture2D dicePower;
    Texture2D bloodOverLay;
    Texture2D texHypnoticGaze;

    Font normalFont;
    Font spookyFont;

    UILabel *locationLabel = nullptr;

    MonsterCard card;
    bool cardDrawn = false;
    float cardScale = 0.0f;
    Vector2 cardPos = {0, 0};

    UIManager ui;
    Game &game = Game::getInstance();

    MonsterPhaseStep currentStep = MonsterPhaseStep::CheckMonsterPhasePerk;
    float stepTimer = 0.0f;
    bool skipPhase = false;

    std::vector<Item *> items;
    std::vector<Texture2D> itemTex;
    bool itemsPlaced = false;
    float itemsTimer = 0.0f;
    float itemsAlpha = 0.0f;
    bool showItemIcons = false;

    bool showBloodOverlay = false;
    float bloodOverlayTimer = 0.0f;

    Hero *powerTargetHero = nullptr;
    Villager *powerTargetVillager = nullptr;
    bool powerVillagerKilled = false;

    bool titelAdded = false;
    bool messageShown = false;

    std::vector<std::string> villagerNames;
    std::vector<Texture2D> villagerTextures;
    bool villagerImagesLoaded = false;

    bool loadedVillager = false;
    bool loadedHypnotic = false;

    bool isWaitingForDefenceSelection = false;
    bool pendingItemSelection = false;
    std::vector<Item *> pendingHeroItems;

    MonsterPhaseStep nextStepAfterPower = MonsterPhaseStep::None;
    std::queue<PowerResult> powerQueue;
    bool processingPowerQueue = false;

    bool waitingForMessage = false;
    std::queue<std::unique_ptr<UILabel>> messageQueue;
    std::queue<float> messageDuration;
    float messageTimer = 0.0f;

    void displayNextMessage();

    std::vector<std::string> strikes;
    bool processingStrike = false;

    std::vector<std::string> rolledDiceResult;
    float diceTimer = 0.0f;
    int diceShown = 0;

    int remainingStrikes = 0;

    void showMessage(const std::string &text, Vector2 pos, int fontSize, float time, Font font, bool immediate, Color color = {235, 235, 235, 255});

    void step_CheckMonsterPhasePerk(float deleteTime);
    void step_DrawMonsterCard(float deleteTime);
    void step_PlaceItems(float deleteTime);
    void step_RunEvent(float deleteTime);
    void step_MoveAndRoll(float deleteTime);
    void step_HandleStrike(float deleteTime);
    void step_HandlePower(float deleteTime);

    void renderBackGround();
    void renderMonsterCard();
    void renderItems();
    void renderEvents();

    void render_FormOfTheBat(float deleteTime);
    void render_Thief(float deleteTime);
    void render_Sunrise(float deleteTime);
    void render_villagerCard(float deleteTime);
    void render_HypnoticGaze(float deleteTime);
    void render_OnTheMove(float deleteTime);

    void handleMoveAndRoll(float deleteTime);
    void handleDefence(std::vector<Item *> &selectedItems);
    void handleManageStrike();

    void renderCurrentMonster();

    void renderDice(float delerteTime);

    void renderPowerEffect();

    void executeStrike();

    void endMonsterPhase();

    void processNextPower();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif