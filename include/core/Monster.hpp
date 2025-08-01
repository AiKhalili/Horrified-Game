#ifndef MONSTER_HPP
#define MONSTER_HPP
#include "core/Item.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <climits>

class Location;
class Hero;
class Villager;
class Dracula;
class Invisible_Man;

enum class TargetType
{
    None,
    Hero,
    Villager
};

class Monster
{
private:
    std::string name;
    Location *location;
    bool defeated;
    int frenzyOrder;

public:
    Monster(std::string);

    //توابع get
    std::string get_name() const;
    Location *get_location() const;
    int getFrenzyOrder() const;
    bool is_defeated() const;
    
    // توابع set
    void set_location(Location *);
    void set_defeated(bool);
    void setFrenzyOrder(int);

    std::vector<Location *> findShortestPath(Location *, Location *);
    virtual Location *moveTowardTarget(std::vector<Hero *>, std::vector<Villager *>, int);
    virtual ~Monster() = default;
    
    virtual std::vector<Location *> getAdvanceLocation() = 0;
    virtual void advanceMission(Location *) = 0;
    virtual bool canbedefeated() const = 0;
    virtual std::vector<Item> getAdvanceRequirement() const = 0;
    virtual std::vector<Item> getDefeatRequirement() const = 0;
    virtual int getCounter() const = 0;
    virtual void specialPower(Hero *) = 0;
    virtual bool isAdvanceLocation(const std::string &) const = 0;

    virtual std::string serialize() const = 0; // مخصوص هر هیولا
    static Monster* deserialize(const std::string&); // تابع اصلی که خط فایل رو می‌خونه و هیولا می‌سازه


    
};

#endif