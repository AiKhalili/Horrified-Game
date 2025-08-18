#ifndef PLAYERINFOSCENE_HPP
#define PLAYERINFOSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UIInputBox.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"
#include <string>
#include <vector>

class PlayerInfoScene : public Scene
{
private:
    UIManager uiManager; // To control buttons and labels
    Texture2D backgroundTexture;
    Font font;
    bool fontLoaded = false; // Flag to check if font is loaded

    UIButton *continueButton = nullptr; // Button to submit player info and continue
    UIButton *backButton = nullptr;     // Button to go back to main menu
    UIButton *focusedButton = nullptr;  // Currently focused button

    UILabel *errorLabel = nullptr; // Currently focused button

    UIInputBox *nameInput = nullptr; // Input box for player's name
    UIInputBox *timeInput = nullptr; // Input box for last garlic-eating time

    UILabel *playerLabel = nullptr; // Label showing which player is entering info

    int currentPlayer = 1; // Current player index (1 or 2)

    void createButtons();    // Creating scene labels
    void createInputBoxes(); // Creates the input boxes for name and time
    void createLabels();     // Creatin scene buttons

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void resetInputsForNextPlayer();               // Clears the input boxes for the next player
    void showErrorMessage(const std::string &msg); // Control the display of various errors
};

#endif
