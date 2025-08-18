#ifndef PERKCARDSCENE_HPP
#define PERKCARDSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "core/Hero.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/TextureManager.hpp"
#include "raylib.h"
#include <string>
#include <vector>

class PerkCardScene : public Scene
{
    Hero *currentHero;
    Texture2D background;
    Font font;
    UIManager uiManager;
    std::vector<Texture2D> perkCardTextures;
    void loadPerkCardsTex();
    void createUI();

public:
    void update(float deleteTime) override;
    void render() override;
    void onEnter() override;
    void onExit() override;
};

#endif