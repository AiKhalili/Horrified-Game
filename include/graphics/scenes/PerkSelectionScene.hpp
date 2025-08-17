#ifndef PERKSELECTION_HPP
#define PERKSELECTION_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/PerkCard.hpp"
#include "core/Hero.hpp"
#include "core/Location.hpp"
#include "raylib.h"

class PerkSelectionScene : public Scene
{

private:
    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font font;
    std::string scenekey; // Key of the previous scene to return to

    std::vector<PerkCard> perks;  // List of available perk cards
    PerkCard selected;            // The perk card finally selected by player
    PerkCard currentSelectedPerk; // Currently hovered/selected perk
    Hero *hero = nullptr;         // Active hero during the game
    Location *location = nullptr; // related location

    UILabel *desLabel = nullptr;   // Label for displaying perk description
    UILabel *errorLabel = nullptr; // Label to show error messages

    std::vector<Texture2D> perkTextures; // Textures for perk cards
    std::vector<Rectangle> perkRects;    // Positions and sizes of perk cards on screen

    bool hasSelected = false;        // Has a perk been selected
    bool hasCurrentSelected = false; // Has a current perk been hovered/selected

    bool needLocationSelection = false;        // Does this perk require location selection
    int requiredLocationCount = 0;             // Number of locations to select (Visit=1, Repel/Hurry=2)
    int currentLocationSelectionCount = 0;     // How many locations already selected
    std::vector<Location *> selectedLocations; // Locations selected for this perk

    int hoveredPerkIndex = -1; // Index of perk currently under mouse

    PerkCard pendingSelectedPerk;    // Card pending for final selection
    bool hasPendingSelected = false; // Is there a pending card

    bool readyToGoNextScene = false;     // Flag to indicate ready to transition
    bool isWaitingToGoNextScene = false; // Waiting for timer before transition
    float timerToNextScene = 0.0f;       // Timer for waiting before scene change
    const float waitDuration = 2.0f;     // Duration to wait before scene change

    bool isReturningFromLocationSelection = false; // Flag for returning from location selection

    void loadPerkTextures(size_t);        // Load textures for perk cards
    void toggleSelection(PerkCard &perk); // Handle selection of a perk card
    void createLabels();                  // Creating scene labels
    void createButtons();                 // Creatin scene buttons
    void creatErroreLabels();             // Create error message label
    void createActionButtons();           // Create "None" and "Use Perk" buttons

    void showDesMessage(PerkCard &perk);   // Display description of selected perk
    void handleLocationSelectionRequest(); // Handle location selection for special perks

    void recalcPerkRects(size_t); // Recalculate positions of perk cards

    void creatHelpLable(bool); // Display helper label for order (heroes/monsters)

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    void setData(const std::vector<PerkCard> &Perks, const std::string &newkey);
    PerkCard &getSelectedPerks();

    int getSelectedPerkIndex();                    // Get index of selected perk in perks vector
    void showErrorMessage(const std::string &msg); // Control the display of various errors

    std::vector<PerkCard> getperks();
    std::string getscenekey();

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};
#endif
