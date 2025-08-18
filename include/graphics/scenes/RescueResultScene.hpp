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

    float elapsedTime = 0.0f;

    float villagerAlpha = 0.0f;
    const float fadeDuration = 3.0f;
    bool villagerFadeDone = false;

    float perkY = -500.0f;
    const float perkTargetY = 430.0f;
    const float slideSpeed = 400.0f;
    bool perkSlideStarted = false;
    bool perkSlideDone = false;

    void loadtextures();
    void createUI();

public:
    void update(float deleteTime) override;
    void render() override;
    void onEnter() override;
    void onExit() override;

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};

#endif