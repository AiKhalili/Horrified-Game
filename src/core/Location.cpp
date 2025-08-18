#include "core/Location.hpp"
#include "core/Hero.hpp"
#include "core/Villager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/Map.hpp"
using namespace std;

Location::Location(string name)
{
    this->name = name;
    this->neighbors = {};
}

string Location::get_name() const { return name; }
vector<Location *> Location::get_neighbors() const { return neighbors; }
vector<Item *> Location::get_items() const { return items; }
vector<Monster *> Location::get_monsters() const { return monsters; }
vector<Hero *> Location::get_heroes() const { return heroes; }
vector<Villager *> Location::get_villagers() const { return villagers; }

vector<Villager *> Location::getNearByVillagers() const
{
    vector<Villager *> nearbyVillagers;

    nearbyVillagers.insert(nearbyVillagers.end(), villagers.begin(), villagers.end());

    for (const auto &neighbor : neighbors)
    {
        if (neighbor)
        {
            const auto &neighborVillagers = neighbor->get_villagers();
            nearbyVillagers.insert(nearbyVillagers.end(), neighborVillagers.begin(), neighborVillagers.end());
        }
    }

    return nearbyVillagers;
}

void Location::addNeighbors(Location *neighbor)
{
    if (find(neighbors.begin(), neighbors.end(), neighbor) == neighbors.end())
    {
        neighbors.push_back(neighbor);
    }
}

void Location::addItem(Item *item)
{
    auto it = find(items.begin(), items.end(), item);
    if (it == items.end())
    {
        items.push_back(item);
        item->set_location(this);
    }
}

void Location::removeItem(Item *item)
{
    auto it = find(items.begin(), items.end(), item);
    if (it != items.end())
    {
        items.erase(it);
    }
    else
    {
        throw GameException("There is no item in this Location!\n");
    }
}

void Location::addMonster(Monster *monster)
{
    auto it = find(monsters.begin(), monsters.end(), monster);
    if (it == monsters.end())
    {
        monsters.push_back(monster);
        monster->set_location(this);
    }
}

void Location::removeMonster(Monster *monster)
{
    auto it = find(monsters.begin(), monsters.end(), monster);
    if (it != monsters.end())
    {
        monsters.erase(it);
    }
    else
    {
        throw GameException("There is no Monster in this Location!\n");
    }
}

void Location::addHero(Hero *hero)
{
    auto it = find(heroes.begin(), heroes.end(), hero);
    if (it == heroes.end())
    {
        heroes.push_back(hero);
        hero->setLocation(this);
    }
}

void Location::removeHero(Hero *hero)
{
    auto it = find(heroes.begin(), heroes.end(), hero);
    if (it != heroes.end())
    {
        heroes.erase(it);
    }
    else
    {
        throw GameException("There is no Hero in this Location!\n");
    }
}

void Location::addVillager(Villager *villager)
{
    auto it = find(villagers.begin(), villagers.end(), villager);
    if (it == villagers.end())
    {
        villagers.push_back(villager);
        villager->setLocation(this);
    }
}

void Location::removeVillager(Villager *villager)
{
    auto it = find(villagers.begin(), villagers.end(), villager);
    if (it != villagers.end())
    {
        villagers.erase(it);
    }
    else
    {
        throw GameException("There is no Villager in this Location!\n");
    }
}

bool Location::isNeighbor(Location *other) const
{
    return find(neighbors.begin(), neighbors.end(), other) != neighbors.end();
}

void Location::clearAll(){
    items.clear();
    monsters.clear();
    villagers.clear();
    heroes.clear();
}

std::string Location::serialize() const {
    std::ostringstream ss;
    ss << "Location|" << name << "\n";
    return ss.str();
}

Location* Location::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string prefix, name;

    getline(ss, prefix, '|');
    getline(ss, name);

    return Map::get_instanse()->getLocation(name);
}
