#ifndef LOCATIONINFOSCENE_HPP
#define LOCATIONINFOSCENE_HPP

#include "core/Location.hpp"
#include "graphics/scenes/Scene.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/ui/UIManager.hpp"
#include "raylib.h"
#include <string>
#include <unordered_map>

class LocationInfoScene : public Scene
{
private:
    Location *currentLocation;
    Font font;
    UIManager ui;
    std::unordered_map<std::string, Texture2D *> textures;

    void loadTextures();
    void createUI();
    void drawCoffin();
    void drawCharacters();
    void drawItems();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;
};

#endif