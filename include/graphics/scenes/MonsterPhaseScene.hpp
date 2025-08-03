#ifndef MONSTERPHASESCENE_HPP
#define MONSTERPHASESCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"
#include <memory>
#include "core/Game.hpp"

enum class MonsterPhaseStep
{
    CheckMonsterPhasePerk,
    DrawMonsterCard,
    PlaceItems,
    RunEvent,
    MonsterMoveAndRoll,
    HandleStrike,
    HandlePower,
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

    Font normalFont;
    Font spookyFont;

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

    bool titelAdded = false;
    bool messageShown = false;

    std::vector<std::string> strikes;
    bool processingStrike = false;

    std::vector<std::string> rolledDiceResult;
    float diceTimer = 0.0f;
    int diceShown = 0;

    int remainingStrikes = 0;

    void showMessage(const std::string &text, Vector2 pos, int fontSize, float time, Font font, Color color = {235, 235, 235, 255});

    void step_CheckMonsterPhasePerk(float deleteTime);
    void step_DrawMonsterCard(float deleteTime);
    void step_PlaceItems(float deleteTime);
    void step_RunEvent(float deleteTime);
    void step_MoveAndRoll(float deleteTime);
    void step_HandleStrike(float deleteTime);

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

    void renderCurrentMonster();

    void renderDice(float delerteTime);

    void executeStrike();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif