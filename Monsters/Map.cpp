#include "Map.hpp"
#include "Hero.hpp"
#include "Villager.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

Map *Map::mapPtr = nullptr;

Map::Map()
{

    locations["Camp"] = new Location("Camp");
    locations["Cave"] = new Location("Cave");
    locations["Precinct"] = new Location("Precinct");
    locations["Inn"] = new Location("Inn");
    locations["Barn"] = new Location("Barn");
    locations["Dungeon"] = new Location("Dungeon");
    locations["Tower"] = new Location("Tower");
    locations["Theatre"] = new Location("Theatre");
    locations["Docks"] = new Location("Docks");
    locations["Institute"] = new Location("Institute");
    locations["Laboratory"] = new Location("Laboratory");
    locations["Shop"] = new Location("Shop");
    locations["Graveyard"] = new Location("Graveyard");
    locations["Church"] = new Location("Church");
    locations["Hospital"] = new Location("Hospital");
    locations["Mansion"] = new Location("Mansion");
    locations["Museum"] = new Location("Museum");
    locations["Crypt"] = new Location("Crypt");
    locations["Abbey"] = new Location("Abbey");

    locations["Camp"]->addNeighbors(locations["Cave"]);
    locations["Camp"]->addNeighbors(locations["Precinct"]);
    locations["Camp"]->addNeighbors(locations["Mansion"]);
    locations["Camp"]->addNeighbors(locations["Inn"]);
    locations["Camp"]->addNeighbors(locations["Theatre"]);

    locations["Cave"]->addNeighbors(locations["Camp"]);

    locations["Precinct"]->addNeighbors(locations["Camp"]);
    locations["Precinct"]->addNeighbors(locations["Inn"]);
    locations["Precinct"]->addNeighbors(locations["Theatre"]);
    locations["Precinct"]->addNeighbors(locations["Mansion"]);

    locations["Inn"]->addNeighbors(locations["Precinct"]);
    locations["Inn"]->addNeighbors(locations["Mansion"]);
    locations["Inn"]->addNeighbors(locations["Theatre"]);
    locations["Inn"]->addNeighbors(locations["Camp"]);

    locations["Barn"]->addNeighbors(locations["Theatre"]);

    locations["Dungeon"]->addNeighbors(locations["Tower"]);

    locations["Tower"]->addNeighbors(locations["Dungeon"]);
    locations["Tower"]->addNeighbors(locations["Theatre"]);
    locations["Tower"]->addNeighbors(locations["Docks"]);

    locations["Theatre"]->addNeighbors(locations["Precinct"]);
    locations["Theatre"]->addNeighbors(locations["Inn"]);
    locations["Theatre"]->addNeighbors(locations["Tower"]);
    locations["Theatre"]->addNeighbors(locations["Shop"]);
    locations["Theatre"]->addNeighbors(locations["Barn"]);
    locations["Theatre"]->addNeighbors(locations["Camp"]);
    locations["Theatre"]->addNeighbors(locations["Mansion"]);

    locations["Docks"]->addNeighbors(locations["Tower"]);

    locations["Institute"]->addNeighbors(locations["Laboratory"]);

    locations["Laboratory"]->addNeighbors(locations["Institute"]);
    locations["Laboratory"]->addNeighbors(locations["Shop"]);

    locations["Shop"]->addNeighbors(locations["Laboratory"]);
    locations["Shop"]->addNeighbors(locations["Theatre"]);
    locations["Shop"]->addNeighbors(locations["Church"]);
    locations["Shop"]->addNeighbors(locations["Mansion"]);
    locations["Shop"]->addNeighbors(locations["Museum"]);

    locations["Graveyard"]->addNeighbors(locations["Church"]);

    locations["Church"]->addNeighbors(locations["Shop"]);
    locations["Church"]->addNeighbors(locations["Graveyard"]);
    locations["Church"]->addNeighbors(locations["Hospital"]);
    locations["Church"]->addNeighbors(locations["Museum"]);
    locations["Church"]->addNeighbors(locations["Mansion"]);

    locations["Hospital"]->addNeighbors(locations["Church"]);

    locations["Mansion"]->addNeighbors(locations["Shop"]);
    locations["Mansion"]->addNeighbors(locations["Museum"]);
    locations["Mansion"]->addNeighbors(locations["Church"]);
    locations["Mansion"]->addNeighbors(locations["Camp"]);
    locations["Mansion"]->addNeighbors(locations["Theatre"]);
    locations["Mansion"]->addNeighbors(locations["Abbey"]);
    locations["Mansion"]->addNeighbors(locations["Precinct"]);
    locations["Mansion"]->addNeighbors(locations["Inn"]);

    locations["Museum"]->addNeighbors(locations["Church"]);
    locations["Museum"]->addNeighbors(locations["Mansion"]);
    locations["Museum"]->addNeighbors(locations["Shop"]);

    locations["Crypt"]->addNeighbors(locations["Abbey"]);

    locations["Abbey"]->addNeighbors(locations["Mansion"]);
    locations["Abbey"]->addNeighbors(locations["Crypt"]);
}

Map *Map::get_instanse()
{
    if (mapPtr == nullptr)
    {
        mapPtr = new Map();
    }
    return mapPtr;
}

Map::~Map()
{
    for (auto &pair : locations)
    {
        if (pair.second != nullptr)
        {
            delete pair.second;
            pair.second = nullptr;
        }
    }
}

Location *Map::getLocation(const std::string &name)
{
    if (locations.count(name))
        return locations[name];
    else
    {
        throw GameException("Location '" + name + "' not found on the map!");
    }
}

void Map::addItemTo(const string &locationName, Item *item)
{
    if (locations.count(locationName))
    {
        locations[locationName]->addItem(item);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

void Map::addMonsterTo(const string &locationName, Monster *monster)
{
    if (locations.count(locationName))
    {
        locations[locationName]->addMonster(monster);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

void Map::removeMonsterFrom(const string &locationName, Monster *monster)
{
    if (locations.count(locationName))
    {
        locations[locationName]->removeMonster(monster);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

void Map::removeItemFrom(const string &locationName, Item *item)
{
    if (locations.count(locationName))
    {
        locations[locationName]->removeItem(item);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

void Map::addHeroTo(const string &locationName, Hero *hero)
{
    if (locations.count(locationName))
    {
        locations[locationName]->addHero(hero);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

void Map::removeHeroFrom(const string &locationName, Hero *hero)
{
    if (locations.count(locationName))
    {
        locations[locationName]->removeHero(hero);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

void Map::addVillager(const string &locationName, Villager *villager)
{
    if (locations.count(locationName))
    {
        locations[locationName]->addVillager(villager);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

void Map::removeVillagerFrom(const string &locationName, Villager *villager)
{
    if (locations.count(locationName))
    {
        locations[locationName]->removeVillager(villager);
    }
    else
    {
        throw GameException("Location '" + locationName + "' not found on the map!");
    }
}

vector<Location *> Map::getAllLocations() const
{
    vector<Location *> allLocations;
    for (const auto &pair : locations)
    {
        allLocations.push_back(pair.second);
    }
    return allLocations;
}

vector<Villager *> Map::getAllVillagers() const
{
    vector<Villager *> allVillagers;

    for (const auto &pair : locations)
    {
        Location *loc = pair.second;
        std::vector<Villager *> villagersAtLoc = loc->get_villagers();
        allVillagers.insert(allVillagers.end(), villagersAtLoc.begin(), villagersAtLoc.end());
    }

    return allVillagers;
}
