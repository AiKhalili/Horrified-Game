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
    Hero *currentHero;
    Texture2D background;
    Font font;
    UIManager uiManager;
    std::unique_ptr<UIButton> uiButton;
    std::vector<Texture2D> itemTextures;
    void loadHeroItems();

public:
    void update(float deleteTime) override;
    void render() override;
    void onEnter() override;
    void onExit() override;
};

#endif