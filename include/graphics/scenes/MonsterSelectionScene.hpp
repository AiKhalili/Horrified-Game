#ifndef MONSTERSELECTIONSCENE_HPP
#define MONSTERSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"

class Game;

class MonsterSelectionScene : public Scene
{
private:
    UIManager ui;
    Texture2D background;
    Font font;

    Texture2D monsterTextures[2];
    void LoadeMonsterTextures();
    void createButtons();

    int hoverMonster = -1;
    int clickedMonster = -1; 
    int confirmedMonster = -1;
    std::string selectedMonsterName;

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    std::string getSelectedMonsterName();
};

#endif