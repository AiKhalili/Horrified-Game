#ifndef INTROSCENE_HPP
#define INTROSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "raylib.h"

class IntroScene : public Scene
{
private:
    Texture2D background;
    float timer = 0.0f;
    float progress = 0.0f;

public:
    IntroScene();
    ~IntroScene();
    void update(float deleteTime) override;
    void render() override;
};

#endif