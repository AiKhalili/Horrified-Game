#ifndef HEROSELECTIONSCENE_HPP
#define HEROSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"

class Game;
constexpr int HERO_COUNT = 4;

class HeroSelectionScene : public Scene
{

private:
    enum class SelectionPhase
    {
        Player1_Select,
        Player1_Submit,
        Player2_Select,
        Player2_Submit,
        Done
    };

    UIManager uiManager;

    SelectionPhase phase;

    Texture2D background;
    Font font;

    Rectangle heroButtonRects[HERO_COUNT];
    Texture2D heroTextures[HERO_COUNT];

    int currentPlayer;
    std::string currentMessage;

    int playerSelections[2] = {-1, -1};

    bool isButtonEnabled(int heroIndex) const;
    void onHeroSelected(int heroIndex);
    std::string heroNameFromIndex(int index) const;

    std::unique_ptr<UIButton> continueButton;
    std::unique_ptr<UIButton> backButton;

    float submitTimer = 0.0f;

    void LoadeHeroTextures();
    void creatButton();

    void whoStart();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    
};

#endif
