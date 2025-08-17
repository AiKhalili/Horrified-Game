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
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    std::string scenekey; // Key of the previous scene to return to

    std::vector<Item *> items;    // Items sent to this scene for selection
    Hero *hero = nullptr;         // Active hero during the game
    Location *location = nullptr; // Location associated with the items

    std::vector<Texture2D> itemTextures; // Loaded textures for each item
    std::vector<Rectangle> itemRects;    // Button rectangles for each item
    std::vector<Item *> selected;        // Items selected by the player

    Rectangle confirmButtonRect;       // Rectangle for the confirm button
    bool confirmButtonFocused = false; // Whether the confirm button is hovered

    UILabel *errorLabel = nullptr; // Label to show error messages

    bool firstDeserialize = true; 

    void loadItemTextures();          // Loads textures for all items in the scene
    void toggleSelection(Item *item); // Toggles selection state of an item (select/unselect)
    void createLabels();              // Creates UI labels for item information and messages
    void createButtons();             // Creates item buttons for selection in the scene
    void creatErroreLabels();         // Creates labels to display error messages
    void createActionButtons();       // Creates action buttons such as Confirm/Cancel for the scene

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void setData(const std::vector<Item *> &Items, const std::string &newkey);
    std::vector<Item *> &getSelectedItems();

    void showErrorMessage(const std::string &msg); // Control the display of various errors

    std::vector<Item *> getItems();
    std::string getscenekey();

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};
#endif
