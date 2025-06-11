#ifndef LOCATION_HPP
#define LOCATION_HPP
#include <iostream>
#include "Item.hpp"
#include "Monster.hpp"
#include "GameException.hpp"
#include <vector>
#include <algorithm>

class Hero;
class Villager;

class Location{
    private:
    std::string name;
    std::vector<Location*> neighbors;
    std::vector<Item*> items;
    std::vector<Monster*> monsters;
    std::vector<Villager*> villagers;
    std::vector<Hero*> heroes;

    public:
    Location(std::string);
    std::string get_name() const;
    std::vector<Location*> get_neighbors() const;
    std::vector<Item*> get_items() const;
    std::vector<Monster*> get_monsters() const;
    std::vector<Hero*> get_heroes() const;
    std::vector<Villager*> get_villagers() const;
    void addNeighbors(Location*);
    void addItem(Item*);
    void removeItem(Item*);
    void addMonster(Monster*);
    void removeMonster(Monster*);
    void addHero(Hero*);
    void removeHero(Hero*);
    void addVillager(Villager*);
    void removeVillager(Villager*);
    bool isNeighbor(Location*) const;

};
#endif