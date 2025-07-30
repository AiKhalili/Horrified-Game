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
    DrawMonsterCard
};

class MonsterPhaseScene : public Scene
{
private:
    Texture2D background;
    Font font;
    UIManager ui;
    Game &game = Game::getInstance();
    MonsterPhaseStep currentStep = MonsterPhaseStep::CheckMonsterPhasePerk;
    float stepTimer = 0.0f;
    bool skipPhase = false;

    void step_CheckMonsterPhasePerk(float deleteTime);

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif