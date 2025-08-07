#ifndef MONSTERSSUMMARYSCENE_HPP
#define MONSTERSSUMMARYSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"

class MonstersSummaryScene : public Scene
{

private:
    UIManager ui;
    Texture2D background;
    Font Titlefont;
    Font Simplefont;
    std::vector<Texture2D> monsterassets;

    UILabel *errorLabel = nullptr;

    void createLabels();
    void creatButtons();
    void LoadMonsterAssetsTexturs();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void showErrorMessage(const std::string &msg);
};
#endif