#ifndef MAP_HPP
#define MAP_HPP
#include "core/Location.hpp"
#include <unordered_map>
#include <vector>
#include <queue>
#include <iostream>

class Map{
private:
    static Map* mapPtr;
    Map();
    std::unordered_map<std::string, Location*> locations; //مکان های این شهر

    public:
    static Map* get_instanse();
    ~Map();
    Location* getLocation(const std::string&);
    std::vector<Location*> getAllLocations() const;
    std::vector<Villager*> getAllVillagers() const;

    void addItemTo(const std::string&, Item*);
    void removeItemFrom(const std::string&, Item*);
    void addMonsterTo(const std::string&, Monster*);
    void removeMonsterFrom(const std::string&, Monster*);
    void addHeroTo(const std::string&, Hero*);
    void removeHeroFrom(const std::string&, Hero*);
    void addVillager(const std::string&, Villager*);
    void removeVillagerFrom(const std::string&, Villager*);    
};

#endif