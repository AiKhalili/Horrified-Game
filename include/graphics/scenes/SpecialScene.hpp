#ifndef SPECIALSCENE_HPP
#define SPECIALSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Hero.hpp"
#include "core/Game.hpp"
#include "core/Location.hpp"
#include "core/Item.hpp"
#include "core/SceneDataHub.hpp"
#include "raylib.h"

class SpecialScene : public Scene
{
private:
    UIManager ui;
    Texture2D background;
    Font font;
    Font errorFont;

    void createLabels();
    void createButtons();

    Hero *hero;

    Location *locselect ;
    std::vector<Item *>itemselect ;

    UILabel* errorLabel = nullptr;


public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void showErrorMessage(const std::string &msg);
};
#endif