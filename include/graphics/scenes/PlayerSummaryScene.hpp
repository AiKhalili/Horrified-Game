#ifndef PLAYERSUMMARYSCENE_HPP
#define PLAYERSUMMARYSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"

class PlayerSummaryScene : public Scene
{

private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font Titlefont;
    Font Simplefont;

    std::string p1Name, p2Name, h1Name, h2Name; // Names of players and heroes
    Texture2D hero1Texture;                     // The image of the first hero
    Texture2D hero2Texture;                     // The image of the second hero

    UILabel *errorLabel = nullptr; // Labels to indicate various errors

    void createLabels();    // Creating scene labels
    void creatButtons();    // Creatin scene buttons
    void LoadHeroTexturs(); // Loads hero textures based on their names

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void setData(const std::string &player1Name, const std::string &hero1,
                 const std::string &player2Name, const std::string &hero2);
    std::vector<std::string> getData();

    void showErrorMessage(const std::string &msg); // Control the display of various errors

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};
#endif
