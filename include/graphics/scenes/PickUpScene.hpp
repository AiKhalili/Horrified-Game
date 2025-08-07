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
    UIManager ui;
    Texture2D background;
    Font font;
    Font errorFont;

    Hero *hero;
    std::vector<Item *> itemselect;
    UILabel *errorLabel = nullptr;

    void createLabels();
    void createButtons();


public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void showErrorMessage(const std::string &msg);
};
#endif