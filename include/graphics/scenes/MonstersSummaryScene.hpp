#ifndef MONSTERSSUMMARYSCENE_HPP
#define MONSTERSSUMMARYSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"

class MonstersSummaryScene : public Scene
{

private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font Titlefont;
    Font Simplefont;
    std::vector<Texture2D> monsterassets; // Load images related to Setup and Mat Munster

    UILabel *errorLabel = nullptr; // Label for showing error messages

    void createLabels(); // Creates labels
    void creatButtons(); // Creates buttons
    void LoadMonsterAssetsTexturs();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void showErrorMessage(const std::string &msg); // Show error message on screen.

};
#endif
