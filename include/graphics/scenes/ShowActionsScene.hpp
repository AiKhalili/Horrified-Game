#ifndef SHOWACTIONSSCENE_HPP
#define SHOWACTIONSSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"

class ShowActionsScene : public Scene
{

private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    UILabel *errorLabel = nullptr; // Labels to indicate various errors

    void createLabels();  // Creating scene labels
    void createButtons(); // Creatin scene buttons

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void openMoveAction();    // Handle move action
    void openPickUpAction();  // Handle Pick Up action
    void openDefeatAction();  // Handle Defeat action
    void openHelpAction();    // Handle Help action
    void openSpecialAction(); // Handle Special action
    void openGuideAction();   // Handle Guid action
    void openAdvanceAction(); // Handle Advance action
    void openUsePerkAction(); // Handle Use Perk action

    void showErrorMessage(const std::string &msg); // Control the display of various errors

};

#endif
