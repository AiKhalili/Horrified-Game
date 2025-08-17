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
    // Represents the current step in the hero selection process
    enum class SelectionPhase
    {
        Player1_Select,
        Player1_Submit,
        Player2_Select,
        Player2_Submit,
        Done
    };

    UIManager uiManager; // To control buttons and labels
    Texture2D background;
    Font font;

    SelectionPhase phase; // Current phase of hero selection

    Rectangle heroButtonRects[HERO_COUNT]; // Button rectangles for each hero
    Texture2D heroTextures[HERO_COUNT];    // Loaded textures for each hero

    int currentPlayer;          // Index of the current player
    std::string currentMessage; // Message displayed at the top of the screen

    int playerSelections[2] = {-1, -1}; // Selected hero index for each player

    std::unique_ptr<UIButton> continueButton; // Button to submit the selection
    std::unique_ptr<UIButton> backButton;     // Button to go back to main menu

    float submitTimer = 0.0f;

    bool isButtonEnabled(int heroIndex) const;      // Checks if a specific hero button is currently enabled for selection
    void onHeroSelected(int heroIndex);             // Called when a hero is selected by the current player
    std::string heroNameFromIndex(int index) const; // Gets the hero's name corresponding to a given index
    void LoadeHeroTextures();                       // Loads all hero textures from the assets directory
    void creatButton();                             // Creates the UI buttons for submitting and going back
    void whoStart();                                // Determines which player starts selecting first based on their times

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
};
#endif
