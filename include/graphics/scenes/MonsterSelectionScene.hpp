#ifndef MONSTERSELECTIONSCENE_HPP
#define MONSTERSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Monster.hpp"
#include "raylib.h"

class Game;

class MonsterSelectionScene : public Scene
{
private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    std::string scenekey; // Scene key to return to after selection

    UILabel *errorLabel = nullptr; // Label for showing error messages

    Texture2D monsterTextures[2]; // Loaded textures for each monster

    int hoverMonster = -1;                 // Index of currently hovered monster
    int clickedMonster = -1;               // Index of currently clicked monster
    int confirmedMonster = -1;             // Index of confirmed monster
    Monster *selectedMonster = nullptr;    // Selected monster pointer
    std::vector<Monster *> monsters;       // Monsters available for selection.
    std::vector<std::string> monsterNames; // Names of the monsters
    std::string clickedMonsterName = "";   // Name of the monster clicked by player

    bool firstDeserialize = true; 

    void LoadeMonsterTextures(); // Loads textures for all monster in the scene
    void createButtons();        // Creates buttons in the scene
    void createLabels();         // Creating page labels and specifications for each monster

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void setDate(const std::vector<Monster *> &Monsters, const std::string &newkey);
    Monster *getSelectedMonster();

    void showErrorMessage(const std::string &msg); // Show error message on screen.

    std::vector<Monster *> getMonsters();
    std::string getscenekey();

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};
#endif
