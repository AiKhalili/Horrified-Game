#ifndef MAINMENUSCENE_HPP
#define MAINMENUSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"

class MainMenuScene : public Scene
{
private:
    Texture2D background;
    Font normalFont;
    UIManager ui;
    void startGame();
    void loadGame();
    void exitGame();

public:
    MainMenuScene() = default;
    ~MainMenuScene() = default;
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif