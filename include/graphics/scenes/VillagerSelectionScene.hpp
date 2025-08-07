#ifndef VILLAGERSELECTIONSCENE_HPP
#define VILLAGERSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Villager.hpp"
#include "raylib.h"

class VillagerSelectionScene : public Scene
{

private:
    UIManager ui;
    Texture2D background;
    Font font;
    Font errorFont;

    std::vector<Texture2D> villagerTextures;
    std::vector<Villager *> villagers;
    std::vector<Villager *> selected;
    std::vector<Rectangle> villagerRects;

    std::string scenekey;

    UILabel *errorLabel = nullptr;

    void loadVillagerTextures();
    void toggleSelection(Villager *villager);
    void createLabels();
    void createButtons();
    void creatErroreLabels();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void setData(const std::vector<Villager *> &villagers, const std::string &newkey);
    std::vector<Villager *> &getSelectedVillagers();
    void showErrorMessage(const std::string &msg);
    std::vector<Villager *> getVillagers();
    std::string getscenekey();
};

#endif