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
    UIManager ui;
    Texture2D background;
    Font font;

    std::string scenekey;

    std::vector<PerkCard> perks;
    PerkCard selected;
    PerkCard currentSelectedPerk;
    Hero *hero = nullptr;
    Location *location = nullptr;

    std::vector<Texture2D> perkTextures;
    std::vector<Rectangle> perkRects;

    Rectangle confirmButtonRect;
    bool confirmButtonFocused = false;
    bool hasSelected = false;
    bool hasCurrentSelected = false;

    UILabel *desLabel = nullptr;
    bool needLocationSelection = false;
    int requiredLocationCount = 0; // Visit = 1 ، Reple/Hurry = 2
    int currentLocationSelectionCount = 0;
    std::vector<Location *> selectedLocations;

    int hoveredPerkIndex = -1;

    PerkCard pendingSelectedPerk;
    bool hasPendingSelected = false;

    bool readyToGoNextScene = false;
    bool isWaitingToGoNextScene = false;
    float timerToNextScene = 0.0f;
    const float waitDuration = 2.0f;

     bool isReturningFromLocationSelection = false;

     UILabel *errorLabel = nullptr;

    void loadPerkTextures();
    void toggleSelection(PerkCard &perk);
    void createLabels();
    void createButtons();
    void creatErroreLabels();
    void createActionButtons();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;
    void setData(const std::vector<PerkCard> &Perks, const std::string &newkey);
    PerkCard &getSelectedPerks();

    void showDesMessage(PerkCard &perk);
    void handleLocationSelectionRequest();
    int getSelectedPerkIndex();
    void showErrorMessage(const std::string &msg);

    std::vector<PerkCard> getperks();
    std::string getscenekey();
};
#endif