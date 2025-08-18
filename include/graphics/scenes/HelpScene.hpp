#ifndef HELPSCENE_HPP
#define HELPSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"

class HelpScene : public Scene
{
private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;

    void createLabels();  // Creating scene labels
    void createButtons(); // Creatin scene buttons

    UILabel *helpLabel = nullptr; // Label to show action help

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void openMoveAction();    // Handle move action help
    void openPickUpAction();  // Handle Pick Up action help
    void openDefeatAction();  // Handle Defeat action help
    void openHelpAction();    // Handle Help action help
    void openSpecialAction(); // Handle Special action help
    void openGuideAction();   // Handle Guid action help
    void openAdvanceAction(); // Handle Advence action help
    void openUsePerkAction(); // Handle Use Perk Card action help

    void showHelpMessage(const std::string &msg); // Control the display of action help
};
#endif
