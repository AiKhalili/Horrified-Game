#ifndef LOCATIONSELECTIONSCENE_HPP
#define LOCATIONSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UILabel.hpp"
#include "core/Location.hpp"
#include "raylib.h"
#include <vector>
#include <string>

class LocationSelectionScene : public Scene
{
private:
    // Represents a location on the map with position, radius, and name
    struct LocationMarker
    {
        Vector2 posision;        // Position of the location on the map
        float radius;            // Interaction radius for hover/click
        std::string locatonName; // Name of the location
    };

    UIManager ui; // To control buttons and labels
    Texture2D background;
    Font normalFont;
    Font locationFont;
    std::string scenekey; // Key of the previous scene to return to

    std::vector<Location *> Sentlocations; // Locations passed to this scene for selection
    Location *locationSelect = nullptr;    // Currently selected location

    std::vector<LocationMarker> locations; // Predefined markers with position/radius for rendering
    std::string hoveredLocation;           // Name of the location currently hovered by the mouse

    UILabel *errorLabel = nullptr; // Label for displaying error messages

    bool firstDeserialize = true; 

    void handleClickOnLocation();                   // Handle click events on locations
    void checkLocationHover();                      // Detect if mouse is hovering a location
    void drawGlow();                                // Draw visual glow around selected or hovered location
    bool isLocationActive(const std::string &name); // Check if a location is selectable
    void drawInactiveLocations();                   // Draw inactive locations with gray overlay
    void createButtons();                           // Create scene action buttons
    void createLabels();                            // Create scene labels and instructions

public:
    void onEnter() override;
    void onExit() override;
    void update(float) override;
    void render() override;

    void setData(const std::vector<Location *> &Locations, const std::string &newkey);
    Location *getSelectedLocation();

    void showErrorMessage(const std::string &msg); // Control the display of various errors

    std::vector<Location *> getSentlocations();
    std::string getscenekey();

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;
};
#endif
