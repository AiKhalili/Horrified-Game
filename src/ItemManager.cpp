#include "ItemManager.hpp"
#include "Map.hpp"
#include <ctime>
#include <algorithm>

using namespace std;

ItemManager::ItemManager()
{
    srand(static_cast<unsigned int>(time(nullptr)));
}

ItemManager::~ItemManager()
{
    for (Item *item : bag)
    {
        delete item;
    }
    for (Item *item : usedItems)
    {
        delete item;
    }
}

void ItemManager::initializeItems()
{
    Map *map = Map::get_instanse();

    struct ItemInfo
    {
        string name;
        int strength;
        string location;
        Color color;
    };

    std::vector<ItemInfo> items = {
        {"Dart", 2, "Inn", Color::red},
        {"Fire Poker", 3, "Mansion", Color::red},
        {"Rapier", 5, "Theatre", Color::red},
        {"Shovel", 2, "Graveyard", Color::red},
        {"Torch", 5, "Barn", Color::red},
        {"Pitchfork", 4, "Barn", Color::red},
        {"Rifle", 6, "Barn", Color::red},
        {"Silver Cane", 6, "Shop", Color::red},
        {"Knife", 3, "Docks", Color::red},
        {"Pistol", 6, "Precinct", Color::red},

        {"Research", 2, "Tower", Color::blue},
        {"Telescope", 2, "Mansion", Color::blue},
        {"Searchlight", 2, "Precinct", Color::blue},
        {"Experiment", 2, "Laboratory", Color::blue},
        {"Analysis", 2, "Institute", Color::blue},
        {"Rotenone", 3, "Institute", Color::blue},
        {"Cosmic Ray Diffuser", 3, "Tower", Color::blue},
        {"Nebularium", 3, "Tower", Color::blue},
        {"Monocane Mixture", 3, "Inn", Color::blue},
        {"Fossil", 3, "Camp", Color::blue},

        {"Flower", 2, "Docks", Color::yellow},
        {"Tarot Deck", 3, "Camp", Color::yellow},
        {"Garlic", 2, "Inn", Color::yellow},
        {"Mirrored Box", 3, "Mansion", Color::yellow},
        {"Stake", 3, "Abbey", Color::yellow},
        {"Scroll Of Thoth", 4, "Museum", Color::yellow},
        {"Violin", 3, "Camp", Color::yellow},
        {"Tablet", 3, "Museum", Color::yellow},
        {"Wolfsbane", 4, "Camp", Color::yellow},
        {"Charm", 4, "Camp", Color::yellow}};

    for (const auto &i : items)
    {
        for (int j = 0; j < 2; ++j)
        {
            Location *loc = map->getLocation(i.location);
            Item *item = new Item(i.strength, i.color, i.name, loc);
            if (loc)
                loc->addItem(item);
            bag.push_back(item);
        }
    }
}

Item *ItemManager::getRandomItem()
{
    if (!bag.empty())
    {
        int index = rand() % bag.size();
        Item *item = bag[index];
        bag.erase(bag.begin() + index);
        return item;
    }
    if (!usedItems.empty())
    {
        recycleUsedItemsToBag();
        return getRandomItem();
    }
    return nullptr;
}

void ItemManager::recycleItem(Item *item)
{
    if (item)
    {
        item->set_location(nullptr);
        usedItems.push_back(item);
    }
}

void ItemManager::recycleUsedItemsToBag()
{
    for (Item *item : usedItems)
    {
        bag.push_back(item);
    }
    usedItems.clear();
}

int ItemManager::ramainingItems() const
{
    return static_cast<int>(bag.size());
}

ItemManager &ItemManager::getInstance()
{
    static ItemManager instance;
    return instance;
}