#ifndef END_GAME_SCENE_HPP
#define END_GAME_SCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "core/Game.hpp"
#include "raylib.h"
#include <string>

class EndGameScene : public Scene
{
private:
    Texture2D background;
    Font font;
    UIManager uiManager;
    GameState finalState;
    Color buttonBackgroundColor;
    Color buttonHoverColor;
    Color buttonTextColor;
    Color CadreColor;
    void loadResources();
    void createUI();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
};

#endif