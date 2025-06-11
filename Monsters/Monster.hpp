#ifndef MONSTER_HPP
#define MONSTER_HPP
#include "Item.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <climits>

class Location;
class Hero;
class Villager;

enum class TargetType { None, Hero, Villager };

enum class MoveResult {
    ReachedHero,
    ReachedVillager,
    MovedOnly,
    NoTarget
};

class Monster{
    private:
    std::string name;
    Location* location;
    bool defeated;

    public:
    Monster(std::string);
    std::string get_name() const;
    Location* get_location() const;
    const int get_asMark() const;
    bool is_defeated() const;
    virtual std::vector<Location*> getAdvanceLocation() = 0;
    void set_location(Location*);
    void set_defeated(bool);
    virtual void advanceMission(Location*) = 0;
    virtual bool canbedefeated()const = 0;
    virtual void defeat(Location*) = 0;
    virtual std::vector<Item> getAdvanceRequirement() const = 0;
    virtual void markAdvanceComplete(const std::string&) = 0;
    virtual std::vector<Item> getDefeatRequirement() const = 0;
    virtual int getCounter() const =  0;
    std::vector<Location*> findShortestPath(Location*, Location*);
    MoveResult moveTowardTarget(std::vector<Hero*>, std::vector<Villager*>, int);
    virtual ~Monster() = default;

};





#endif