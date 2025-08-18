#include "core/SceneDataHub.hpp"

SceneDataHub &SceneDataHub::getInstance()
{
    static SceneDataHub instance;
    return instance;
}

void SceneDataHub::reset()
{
    items.clear();
    monster = nullptr;
    villagers.clear();
    location = nullptr;
}

void SceneDataHub::setSelectedItems(const std::vector<Item *> &Items)
{
    items = Items;
}

std::vector<Item *> &SceneDataHub::getSelectedItems()
{
    return items;
}

void SceneDataHub::setSelectedMonster(Monster *Monster)
{
    monster = Monster;
}

Monster *SceneDataHub::getSelectedMonster()
{
    return monster;
}

void SceneDataHub::setSelectedVillagers(const std::vector<Villager *> &Villagers)
{
    villagers = Villagers;
}
std::vector<Villager *> &SceneDataHub::getSelectedVillagers()
{
    return villagers;
}

void SceneDataHub::setSelectedLocation(Location *Location)
{
    location = Location;
}

Location *SceneDataHub::getSelectedLocation()
{
    return location;
}
