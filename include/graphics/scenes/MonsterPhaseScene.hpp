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
    PlaceItems
};

class MonsterPhaseScene : public Scene
{
private:
    Texture2D background;
    Texture2D cardTexture;
    Font font;
    UIManager ui;
    Game &game = Game::getInstance();
    MonsterPhaseStep currentStep = MonsterPhaseStep::CheckMonsterPhasePerk;

    float stepTimer = 0.0f;
    float cardScale = 0.0f;

    bool cardDrawn = false;
    bool skipPhase = false;
    MonsterCard card;

    std::vector<Item*> items;
    std::vector<Texture2D> itemTex;

    void step_CheckMonsterPhasePerk(float deleteTime);
    void step_DrawMonsterCard(float deleteTime);

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif