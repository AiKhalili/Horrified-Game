#include "ItemManager.hpp"
#include "Map.hpp"
#include <ctime>
#include <unordered_set>
#include <algorithm>

using namespace std;

ItemManager::ItemManager()
{
    srand(static_cast<unsigned int>(time(nullptr)));
}

ItemManager::~ItemManager()
{
    clear();
}

void ItemManager::initializeItems()
{
    Map *map = Map::get_instanse();

    struct ItemInfo
    {
        string name;
        int strength;
        string location;
        COlOR color;
    };

    std::vector<ItemInfo> items = {
        {"Dart", 2, "Inn", COlOR::red},
        {"Fire Poker", 3, "Mansion", COlOR::red},
        {"Rapier", 5, "Theatre", COlOR::red},
        {"Shovel", 2, "Graveyard", COlOR::red},
        {"Torch", 5, "Barn", COlOR::red},
        {"Pitchfork", 4, "Barn", COlOR::red},
        {"Rifle", 6, "Barn", COlOR::red},
        {"Silver Cane", 6, "Shop", COlOR::red},
        {"Knife", 3, "Docks", COlOR::red},
        {"Pistol", 6, "Precinct", COlOR::red},

        {"Research", 2, "Tower", COlOR::blue},
        {"Telescope", 2, "Mansion", COlOR::blue},
        {"Searchlight", 2, "Precinct", COlOR::blue},
        {"Experiment", 2, "Laboratory", COlOR::blue},
        {"Analysis", 2, "Institute", COlOR::blue},
        {"Rotenone", 3, "Institute", COlOR::blue},
        {"Cosmic Ray Diffuser", 3, "Tower", COlOR::blue},
        {"Nebularium", 3, "Tower", COlOR::blue},
        {"Monocane Mixture", 3, "Inn", COlOR::blue},
        {"Fossil", 3, "Camp", COlOR::blue},

        {"Flower", 2, "Docks", COlOR::yellow},
        {"Tarot Deck", 3, "Camp", COlOR::yellow},
        {"Garlic", 2, "Inn", COlOR::yellow},
        {"Mirrored Box", 3, "Mansion", COlOR::yellow},
        {"Stake", 3, "Abbey", COlOR::yellow},
        {"Scroll Of Thoth", 4, "Museum", COlOR::yellow},
        {"Violin", 3, "Camp", COlOR::yellow},
        {"Tablet", 3, "Museum", COlOR::yellow},
        {"Wolfsbane", 4, "Camp", COlOR::yellow},
        {"Charm", 4, "Camp", COlOR::yellow}};

    for (const auto &i : items)
    {
        for (int j = 0; j < 2; ++j)
        {
            Location *loc = map->getLocation(i.location);
            Item *item = new Item(i.strength, i.color, i.name, loc);
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

void ItemManager::recycleItemToUsedItems(Item *item)
{
    if (item)
    {
        // حذف از bag اگه وجود داره
        auto it = find(bag.begin(), bag.end(), item);
        if (it != bag.end())
        {
            bag.erase(it);
        }

        // اضافه به usedItems اگه قبلاً اضافه نشده
        if (find(usedItems.begin(), usedItems.end(), item) == usedItems.end())
        {
            item->set_location(nullptr);
            usedItems.push_back(item);
        }
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

vector<Item *> ItemManager::findItemsAtLocation(const vector<string> &locationNames)
{
    vector<Item *> result;

    for (size_t i = 0; i < locationNames.size(); i++)
    {
        for (size_t j = 0; j < bag.size(); j++)
        {
            if (locationNames[i] == bag[j]->get_location()->get_name())
            {
                result.push_back(bag[j]);
                bag.erase(bag.begin() + j);
                break;
            }
        }
    }

    return result;
}

void ItemManager::clear()
{
    std::unordered_set<Item *> deleted;

    for (Item *&item : bag)
    {
        if (item && deleted.find(item) == deleted.end())
        {
            delete item;
            deleted.insert(item);
            item = nullptr;
        }
    }
    bag.clear();

    for (Item *&item : usedItems)
    {
        if (item && deleted.find(item) == deleted.end())
        {
            delete item;
            deleted.insert(item);
            item = nullptr;
        }
    }
    usedItems.clear();
}