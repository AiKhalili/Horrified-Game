#ifndef ITEMSELECTIONSCENE_HPP
#define ITEMSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Item.hpp"
#include "core/Hero.hpp"
#include "core/Location.hpp"
#include "raylib.h"

class ItemSelectionScene : public Scene
{

private:
    UIManager ui;
    Texture2D background;
    Font font;

    std::string scenekey;

    std::vector<Item *> items;
    Hero *hero = nullptr;
    Location *location = nullptr;

    std::vector<Texture2D> itemTextures;
    std::vector<Rectangle> itemRects;
    std::vector<Item *> selected;

    Rectangle confirmButtonRect;
    bool confirmButtonFocused = false;

    UILabel *errorLabel = nullptr;

    void loadItemTextures();
    void toggleSelection(Item *item);
    void createLabels();
    void createButtons();
    void creatErroreLabels();
    void createActionButtons();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void setData(const std::vector<Item *> &Items, const std::string &newkey);
    std::vector<Item *> &getSelectedItems();
    void showErrorMessage(const std::string &msg);
    std::vector<Item *> getItems();
    std::string getscenekey();
};

#endif