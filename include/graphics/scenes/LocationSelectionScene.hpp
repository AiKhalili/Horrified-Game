#ifndef LOCATIONSELECTIONSCENE_HPP
#define LOCATIONSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "core/Location.hpp"
#include "raylib.h"
#include <vector>
#include <string>

class LocationSelectionScene : public Scene
{
private:
    struct LocationMarker
    {
        Vector2 posision;
        float radius;
        std::string locatonName;
    };

    Texture2D background;
    Font normalFont;
    Font locationFont;

    std::vector<Location *> Sentlocations;
    Location *locationSelect = nullptr;

    UIManager ui;
    std::vector<LocationMarker> locations;
    std::string hoveredLocation;

    void handleClickOnLocation();
    void checkLocationHover();
    void drawGlow();
    bool isLocationActive(const std::string &name);
    void drawInactiveLocations();
    void createButtons();
    void createLabels();
    
public:
    void onEnter() override;
    void onExit() override;
    void update(float) override;
    void render() override;
    void setData(const std::vector<Location *> &Locations);
    Location *getSelectedLocation();
};
#endif