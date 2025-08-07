#ifndef DEFEATSCENE_HPP
#define DEFEATSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Hero.hpp"
#include "core/Game.hpp"
#include "core/Monster.hpp"
#include "core/Item.hpp"
#include "core/SceneDataHub.hpp"
#include "raylib.h"

class DefeatScene : public Scene
{
private:
    UIManager ui;
    Texture2D background;
    Font font;
    Font errorFont;

    void createLabels();
    void createButtons();

    Hero *hero;

    Monster *monselect = nullptr ;
    std::vector<Item *>itemselect ;

    UILabel* errorLabel = nullptr;

    struct Particle
    {
        Vector2 position;
        Vector2 velocity;
        float lifetime;
        float alpha;
    };

    std::vector<Particle> abilityParticles;
    bool abilityEffectActive = false;
    float abilityEffectTimer = 0.0f;

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void showErrorMessage(const std::string &msg);
};
#endif