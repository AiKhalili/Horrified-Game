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

    // توابع get
    std::string get_name() const;
    std::vector<Location*> get_neighbors() const;
    std::vector<Item*> get_items() const;
    std::vector<Monster*> get_monsters() const;
    std::vector<Hero*> get_heroes() const;
    std::vector<Villager*> get_villagers() const;
    std::vector<Villager*> getNearByVillagers() const;

    void addNeighbors(Location*); //افزودن همسایه اون مکان
    void addItem(Item*); // افزودن آیتم اون مکان
    void addMonster(Monster*);//افزودن هیولا اون مکان
    void addHero(Hero*);//افزودن قهرمان اون مکان
    void addVillager(Villager*);//افزودن محلی اون مکان
    void removeItem(Item*); // حذف آیتم از اون مکان
    void removeMonster(Monster*); // حذف هیولااز اون مکان
    void removeHero(Hero*);// حذف قهرمان از اون مکان
    void removeVillager(Villager*);// حذف محلی از اون مکان

    bool isNeighbor(Location*) const;
    void clearAll();

};
#endif