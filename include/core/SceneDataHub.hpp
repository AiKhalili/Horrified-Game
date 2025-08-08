#ifndef SCENEDATAHUB_HPP
#define SCENEDATAHUB_HPP

#include "core/Item.hpp"
#include "core/Monster.hpp"
#include "core/Villager.hpp"
#include "core/Location.hpp"
#include <vector>
#include <string>

class SceneDataHub
{
private:
    std::vector<Item *> items;
    Monster *monster = nullptr;
    std::vector<Villager *> villagers;
    Location *location = nullptr;
    SceneDataHub() = default;

public:
    static SceneDataHub &getInstance();
    SceneDataHub(const SceneDataHub &) = delete;
    SceneDataHub &operator=(const SceneDataHub &) = delete;

    void reset();

    void setSelectedItems(const std::vector<Item *> &Items);
    std::vector<Item *> &getSelectedItems();

    void setSelectedMonster(Monster *monster);
    Monster *getSelectedMonster();

    void setSelectedVillagers(const std::vector<Villager *> &Villagers);
    std::vector<Villager *> &getSelectedVillagers();

    void setSelectedLocation(Location *location);
    Location *getSelectedLocation();
};

#endif