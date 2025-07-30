#ifndef VILLAGERSELECTIONSCENE_HPP
#define VILLAGERSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "core/Villager.hpp"
#include "raylib.h"

class VillagerSelectionScene : public Scene
{

private:
    UIManager ui;
    Texture2D background;
    Font font;

    std::vector<Texture2D> villagerTextures;
    std::vector<Villager *> villagers;
    std::vector<Villager *> selected;
    std::vector<Rectangle> villagerRects;

    void loadVillagerTextures();
    void toggleSelection(Villager *villager);
    void createLabels();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void setData(const std::vector<Villager *> &villagers);
    std::vector<Villager *> &getSelectedVillagers();
};

#endif