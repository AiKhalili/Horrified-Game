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
public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void resetInputsForNextPlayer();
    void showErrorMessage(const std::string &msg);

private:
    UIManager uiManager;
    Texture2D backgroundTexture;
    Font font;
    bool fontLoaded = false;

    UIButton *continueButton = nullptr;
    UIButton *backButton = nullptr;
    UIButton *focusedButton = nullptr;

    UILabel* errorLabel = nullptr;

    UIInputBox *nameInput = nullptr;
    UIInputBox *timeInput = nullptr;

    UILabel* playerLabel = nullptr;

    int currentPlayer = 1;

    void createButtons();
    void createInputBoxes();
    void createLabels();

    void debugCheckUIElements();
};

#endif
