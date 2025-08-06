#ifndef RESCUERESULTSCENE_HPP
#define RESCUERESULTSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "core/Hero.hpp"
#include "core/Villager.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/TextureManager.hpp"
#include "raylib.h"
#include <string>

class RescueResultScene : public Scene
{
    Hero *currentHero;
    Villager *rescuedVillager;
    PerkCard perkCard;

    Texture2D background, villagerTex, perkTex;
    Font font;
    UIManager uiManager;

    void loadtextures();
    void createUI();

public:
    void update(float deleteTime) override;
    void render() override;
    void onEnter() override;
    void onExit() override;
};

#endif