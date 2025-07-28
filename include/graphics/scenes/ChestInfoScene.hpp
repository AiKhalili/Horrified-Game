#ifndef CHESTINFOSCENE_HPP
#define CHESTINFOSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "core/Hero.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/TextureManager.hpp"
#include "raylib.h"
#include <string>
#include <vector>
#include <memory>

class ChestInfoScene : public Scene
{
private:
    struct HeroItemTex
    {
        Texture2D tex;
        COlOR color;
    };

    Hero *currentHero;
    Texture2D background;
    Font font;
    UIManager uiManager;
    std::vector<HeroItemTex> itemTextures;
    void loadHeroItems();
    void createUI();

public:
    void update(float deleteTime) override;
    void render() override;
    void onEnter() override;
    void onExit() override;
};

#endif