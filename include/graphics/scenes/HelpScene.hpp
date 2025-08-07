#ifndef HELPSCENE_HPP
#define HELPSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"

class HelpScene : public Scene
{

private:
    UIManager ui;
    Texture2D background;
    Font font;
    void createLabels();
    void createButtons();

    UILabel* helpLabel = nullptr;

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

    void showHelpMessage(const std::string &msg);

};
#endif