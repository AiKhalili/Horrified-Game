#ifndef MONSTERSELECTIONSCENE_HPP
#define MONSTERSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "core/Monster.hpp"
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
    void createLabels();

    int hoverMonster = -1;
    int clickedMonster = -1; 
    int confirmedMonster = -1;
    Monster* selectedMonster = nullptr;
    std::vector<Monster *> monsters;
    std::vector<std::string> monsterNames;
    std::string clickedMonsterName = "";
    
public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void setDate(std::vector<Monster *> &Monsters);
    Monster* getSelectedMonster();
};

#endif