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
    struct ItemTex
    {
        Texture2D tex;
        COlOR color;
    };

    Texture2D background;
    Location *currentLocation = nullptr;
    Font font;
    UIManager ui;

    std::vector<ItemTex> itemTextures;
    std::vector<Texture2D> heroTextures;
    std::vector<Texture2D> monsterTextures;
    std::vector<Texture2D> villagerTextures;

    void loadTextures();
    void createUI();
    void drawCoffin();
    void drawHeroAndMonster();
    void drawVillagers();
    void drawItems();

public:
    void onEnter() override;
    void onExit() override;
    void update(float deleteTime) override;
    void render() override;

    void serialize(const std::string &filename) override;
    void deserialize(const std::string &filename) override;

    void setLocation(Location *loc);
};

#endif