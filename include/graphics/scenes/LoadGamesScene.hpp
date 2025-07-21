#ifndef SHOWLOADGAMESSCENE_HPP
#define SHOWLOADGAMESSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"

class LoadGamesScene : public Scene
{
private:
    Texture2D background;
    Font normalFont;
    UIManager ui;

    void createButtons();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif