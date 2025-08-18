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
    Game &game = Game::getInstance();
    MonsterPhaseStep currentStep = MonsterPhaseStep::CheckMonsterPhasePerk;
    bool skipPhase = false;
    float stepTimer = 0.0f;

    Texture2D background;
    Texture2D cardTexture, bats, frenzy, bloodOverLay;
    Texture2D diceBlank, diceStrike, dicePower;
    Texture2D texHypnoticGaze;
    Texture2D villagerTex;

    Font normalFont, spookyFont;

    MonsterCard card;
    bool cardDrawn = false;
    float cardScale = 0.0f;
    Vector2 cardPos = {0, 0};

    UIManager ui;
    UILabel *locationLabel = nullptr;
    std::queue<std::unique_ptr<UILabel>> messageQueue;
    std::queue<float> messageDuration;
    float messageTimer = 0.0f;
    bool waitingForMessage = false;
    bool titelAdded = false;
    bool messageShown = false;

    std::vector<std::string> rolledDiceResult;
    std::vector<std::string> strikes;
    bool processingStrike = false;
    float diceTimer = 0.0f;
    int diceShown = 0;
    int remainingStrikes = 0;

    std::queue<PowerResult> powerQueue;
    Hero *powerTargetHero = nullptr;
    Villager *powerTargetVillager = nullptr;
    bool powerVillagerKilled = false;
    bool processingPowerQueue = false;

    std::vector<Item *> items;
    std::vector<Texture2D> itemTex;
    bool itemsPlaced = false;
    bool showItemIcons = false;
    float itemsTimer = 0.0f;
    float itemsAlpha = 0.0f;

    std::vector<std::string> villagerNames;
    std::vector<Texture2D> villagerTextures;
    bool villagerImagesLoaded = false;
    bool loadedVillager = false;

    bool loadedHypnotic = false;
    bool showBloodOverlay = false;
    bool batSoundPlayed = false;
    float bloodOverlayTimer = 0.0f;

    bool isWaitingForDefenceSelection = false;
    bool pendingItemSelection = false;
    std::vector<Item *> pendingHeroItems;

    void showMessage(const std::string &text, Vector2 pos, int fontSize, float time,
                     Font font, bool immediate, bool centerAlignment = false, Color color = {235, 235, 235, 255});
    void displayNextMessage();

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
    void renderCurrentMonster();
    void renderDice(float delerteTime);
    void renderPowerEffect();

    void render_FormOfTheBat(float deleteTime);
    void render_Thief(float deleteTime);
    void render_Sunrise(float deleteTime);
    void render_villagerCard(float deleteTime);
    void render_HypnoticGaze(float deleteTime);
    void render_OnTheMove(float deleteTime);

    void handleMoveAndRoll(float deleteTime);
    void handleDefence(std::vector<Item *> &selectedItems);
    void handleManageStrike();
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