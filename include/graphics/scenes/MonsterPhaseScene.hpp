#ifndef MONSTERPHASESCENE_HPP
#define MONSTERPHASESCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"
#include "core/Game.hpp"

enum class MonsterPhaseStep
{
    CheckMonsterPhasePerk,
    DrawMonsterCard,
    PlaceItems,
    RunEvent
};

class MonsterPhaseScene : public Scene
{
private:
    Texture2D background;
    Texture2D cardTexture;
    Texture2D bats;
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

    void showMessage(const std::string& text,Vector2 pos,int fontSize,float time);

    void step_CheckMonsterPhasePerk(float deleteTime);
    void step_DrawMonsterCard(float deleteTime);
    void step_PlaceItems(float deleteTime);
    void step_RunEvent(float deleteTime);

    void renderBackGround();
    void renderMonsterCard();
    void renderItems();

    void render_FormOfTheBat(float deleteTime);

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif