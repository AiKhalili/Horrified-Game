#include "core/ItemManager.hpp"
#include "core/Map.hpp"
#include <ctime>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <sstream>

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
        {"FirePoker", 3, "Mansion", COlOR::red},
        {"Rapier", 5, "Theatre", COlOR::red},
        {"Shovel", 2, "Graveyard", COlOR::red},
        {"Torch", 5, "Barn", COlOR::red},
        {"Pitchfork", 4, "Barn", COlOR::red},
        {"Rifle", 6, "Barn", COlOR::red},
        {"SilverCane", 6, "Shop", COlOR::red},
        {"Knife", 3, "Docks", COlOR::red},
        {"Pistol", 6, "Precinct", COlOR::red},

        {"Research", 2, "Tower", COlOR::blue},
        {"Telescope", 2, "Mansion", COlOR::blue},
        {"Searchlight", 2, "Precinct", COlOR::blue},
        {"Experiment", 2, "Laboratory", COlOR::blue},
        {"Analysis", 2, "Institute", COlOR::blue},
        {"Rotenone", 3, "Institute", COlOR::blue},
        {"CosmicRayDiffuser", 3, "Tower", COlOR::blue},
        {"Nebularium", 3, "Tower", COlOR::blue},
        {"MonocaneMixture", 3, "Inn", COlOR::blue},
        {"Fossil", 3, "Camp", COlOR::blue},

        {"Flower", 2, "Docks", COlOR::yellow},
        {"TarotDeck", 3, "Camp", COlOR::yellow},
        {"Garlic", 2, "Inn", COlOR::yellow},
        {"MirroredBox", 3, "Mansion", COlOR::yellow},
        {"Stake", 3, "Abbey", COlOR::yellow},
        {"ScrollOfThoth", 4, "Museum", COlOR::yellow},
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

void ItemManager::clear()
{
    unordered_set<Item *> deleted;

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

std::string ItemManager::serializeBag() const
{
    std::string result = "ItemBag|";
    for (const Item *item : bag)
    {
        result += item->get_name() + "-" +
                  std::to_string(item->get_strength()) + "-" +
                  item->get_color_to_string() + "-" +
                  item->get_pickedFrom() + ",";
    }
    return result;
}

std::string ItemManager::serializeUsed() const
{
    std::string result = "ItemUsed|";
    for (const Item *item : usedItems)
    {
        result += item->get_name() + "-" +
                  std::to_string(item->get_strength()) + "-" +
                  item->get_color_to_string() + "-" +
                  item->get_pickedFrom() + ",";
    }
    return result;
}

void ItemManager::deserializeBag(const std::string &line)
{
    bag.clear();
    if (line.rfind("ItemBag|", 0) != 0)
        return;

    std::string content = line.substr(8); // بعد از "ItemBag|"
    std::stringstream ss(content);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        if (token.empty())
            continue;

        std::stringstream part(token);
        std::string name, str, colorStr, pickedFrom;

        std::getline(part, name, '-');
        std::getline(part, str, '-');
        std::getline(part, colorStr, '-');
        std::getline(part, pickedFrom, '-');

        COlOR color = Item::stringToColor(colorStr);
        Location *loc = Map::get_instanse()->getLocation(pickedFrom);

        Item *item = new Item(std::stoi(str), color, name, loc);
        item->set_pickedFrom(pickedFrom);
        bag.push_back(item);
    }
}

void ItemManager::deserializeUsed(const std::string &line)
{
    usedItems.clear();
    if (line.rfind("ItemUsed|", 0) != 0)
        return;

    std::string content = line.substr(9); // بعد از "ItemUsed|"
    std::stringstream ss(content);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        if (token.empty())
            continue;

        std::stringstream part(token);
        std::string name, str, colorStr, pickedFrom;

        std::getline(part, name, '-');
        std::getline(part, str, '-');
        std::getline(part, colorStr, '-');
        std::getline(part, pickedFrom, '-');

        COlOR color = Item::stringToColor(colorStr);
        Location *loc = Map::get_instanse()->getLocation(pickedFrom);

        Item *item = new Item(std::stoi(str), color, name, loc);
        item->set_pickedFrom(pickedFrom);
        usedItems.push_back(item);
    }
}
