#ifndef GUIDSCENE_HPP
#define GUIDSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Hero.hpp"
#include "core/Game.hpp"
#include "core/Location.hpp"
#include "core/Villager.hpp"
#include "core/SceneDataHub.hpp"
#include "raylib.h"

class GuidScene : public Scene
{
private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    Font errorFont;

    void createLabels();  // Creating scene labels
    void createButtons(); // Creatin scene buttons

    Hero *hero;        // Active hero during the game
    Location *villloc; // Villager location

    Location *locselect;                // Selected location for villager relocation
    std::vector<Villager *> villselect; // The chosen villager

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
