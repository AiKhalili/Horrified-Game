#ifndef PLAYERINFOSCENE_HPP
#define PLAYERINFOSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/ui/UIInputBox.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "raylib.h"
#include <string>
#include <vector>
#include <memory>

class PlayerInfoScene : public Scene
{
public:
    PlayerInfoScene();
    ~PlayerInfoScene();

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void resetInputsForNextPlayer();
    void showErrorMessage(const std::string &msg);
    void updateBackButtonVisibility();

private:
    UIManager uiManager;
    Texture2D backgroundTexture;
    Font font;

    UIButton* continueButton = nullptr;
    UIButton* backButton = nullptr;

    UIInputBox* nameInput = nullptr;
    UIInputBox* timeInput = nullptr;

    int currentPlayer;
    std::vector<std::string> playerNames;
    std::vector<std::string> playerTimes;

    std::string errorMessage;
    float errorDisplayTime = 0.0f;
};

#endif
