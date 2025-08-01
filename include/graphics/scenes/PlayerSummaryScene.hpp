#ifndef PLAYERSUMMARYSCENE_HPP
#define PLAYERSUMMARYSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"

class PlayerSummaryScene : public Scene
{

private:
    UIManager ui;
    Texture2D background;
    Font Titlefont;
    Font Simplefont;
    std::string p1Name, p2Name, h1Name, h2Name;
    Texture2D hero1Texture;
    Texture2D hero2Texture;

    void createLabels();
    void creatButtons();
    void LoadHeroTexturs();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void setData(const std::string &player1Name, const std::string &hero1,
                 const std::string &player2Name, const std::string &hero2);
};
#endif