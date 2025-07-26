#ifndef BOARDSCENE_HPP
#define BOARDSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"
#include <vector>
#include <string>

class BoardScene : public Scene
{
private:
    struct LocationMarker
    {
        Vector2 posision;
        float radius;
        std::string locatonName;
    };

    Texture2D background;
    Texture2D skullIcon;
    Texture2D chestIcon;
    Texture2D cardIcon;
    Texture2D greenFlagIcon;

    Font normalFont;
    Font locationFont;

    UIManager ui;
    std::vector<LocationMarker> locations;
    std::string hoveredLocation;

    void handleClickOnLocation();
    void handleHeroInfoClick();
    void checkLocationHover();
    void DrawTerrorLevel();
    void drawHeroInfo();
    void drawHeroPositionMarker();
    void drawGlow();
    void drawHeroActions();

    void handleSelectAction();
    void handleSaveAndExit();
    void handleGoToMainMenu();
    void handleExitGame();
    void handleEndHeroPhase();
    void handleGameStateTransition();

    void makeButton(const std::string &text, int row, int col, std::function<void()> onClick, bool center = false);

public:
    void onEnter() override;
    void onExit() override;
    void update(float) override;
    void render() override;
};

#endif