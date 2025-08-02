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
    UIManager ui;
    Texture2D background;
    Font font;
    Font errorFont;

    void createLabels();
    void createButtons();

    Hero *hero;
    Location *location;

    Location *locselect ;
    std::vector<Villager *>villselect ;

    UILabel* errorLabel = nullptr;


public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void showErrorMessage(const std::string &msg);
};
#endif