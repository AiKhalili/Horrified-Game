#ifndef ADVANCESCENE_HPP
#define ADVANCESCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Hero.hpp"
#include "core/Game.hpp"
#include "core/Monster.hpp"
#include "core/Item.hpp"
#include "core/SceneDataHub.hpp"
#include "raylib.h"

class AdvanceScene : public Scene
{
private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    Font errorFont;

    void createLabels();  // Creating scene labels
    void createButtons(); // Creatin scene buttons

    Hero *hero; // Active hero during the game

    Monster *monselect = nullptr;   // The chosen monster
    std::vector<Item *> itemselect; // The chosen items

    UILabel *errorLabel = nullptr; // Labels to indicate various errors

    // To show the effect when pressing Advance
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

    void showErrorMessage(const std::string &msg); // Control the display of various errors

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};
#endif
