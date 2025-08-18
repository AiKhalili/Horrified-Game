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
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    Font errorFont;
    std::string scenekey; // Key of the previous scene to return to

    std::vector<Texture2D> villagerTextures; // Textures of all villagers
    std::vector<Villager *> villagers;       // List of all villagers in this scene
    std::vector<Villager *> selected;        // The chosen villagers
    std::vector<Rectangle> villagerRects;    // Rectangles to display villagers on screen

    UILabel *errorLabel = nullptr; // Labels to indicate various errors

    bool firstDeserialize = true; 

    void loadVillagerTextures();              // Loads textures for each villager from file
    void toggleSelection(Villager *villager); // Toggles selection state for a villager
    void createLabels();                      // Creating scene labels
    void createButtons();                     // Creatin scene buttons
    void creatErroreLabels();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void setData(const std::vector<Villager *> &villagers, const std::string &newkey);
    std::vector<Villager *> &getSelectedVillagers();

    void showErrorMessage(const std::string &msg); // Control the display of various errors

    std::vector<Villager *> getVillagers();
    std::string getscenekey();

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};
#endif
