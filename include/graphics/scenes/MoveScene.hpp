#ifndef MOVESCENE_HPP
#define MOVESCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Hero.hpp"
#include "core/Game.hpp"
#include "core/Location.hpp"
#include "core/SceneDataHub.hpp"
#include "raylib.h"

class MoveScene : public Scene
{
private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    Font errorFont;

    void createLabels();  // Creating scene labels
    void createButtons(); // Creatin scene buttons

    Hero *hero;         // Active hero during the game
    Location *location; // Active hero location during the game

    Location *locselect;                // Selected location for movement
    std::vector<Villager *> villselect; // Villagers of choice to take with you

    UILabel *errorLabel = nullptr; // Labels to indicate various errors

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
