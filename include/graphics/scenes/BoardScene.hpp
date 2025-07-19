#ifndef BOARDSCENE_HPP
#define BOARDSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"
#include <vector>
#include <string>

struct LocarionMarker
{
    Vector2 posision;
    float radius;
    std::string locatonName;
};

class BoardScene : public Scene
{
private:
    Texture2D background;
    UIManager ui;
    Font normalFont;
    std::vector<LocarionMarker> locations;

public:
    void onEnter() override;
    void onExit() override;
    void update(float) override;
    void render() override;
};

#endif