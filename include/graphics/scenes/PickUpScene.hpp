#ifndef PICKUPSCENE_HPP
#define PICKUPSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Hero.hpp"
#include "core/Item.hpp"
#include "core/Game.hpp"
#include "core/SceneDataHub.hpp"
#include "raylib.h"

class PickUpScene : public Scene
{

private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    Font errorFont;

    Hero *hero;                     // Active hero during the game
    std::vector<Item *> itemselect; // The chosen items
    UILabel *errorLabel = nullptr;  // Labels to indicate various errors

    void createLabels();  // Creating scene labels
    void createButtons(); // Creatin scene buttons

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
