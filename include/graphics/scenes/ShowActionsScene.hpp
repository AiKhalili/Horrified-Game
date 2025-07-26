#ifndef SHOWACTIONSSCENE_HPP
#define SHOWACTIONSSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"

class ShowActionsScene : public Scene
{

private:
    UIManager ui;
    Texture2D background;
    Font font;
    void createLabels();
    void createButtons();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void openMoveAction();
    void openPickUpAction();
    void openDefeatAction();
    void openHelpAction();
    void openSpecialAction();
    void openGuideAction();
    void openAdvanceAction();
    void openUsePerkAction();
};

#endif