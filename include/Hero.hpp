#ifndef HERO_HPP
#define HERO_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "Item.hpp"
#include "GameException.hpp"
#include "Villager.hpp"
#include "Location.hpp"
#include "Monster.hpp"
#include "ItemManager.hpp"

class Hero
{
protected:
    std::string name;
    Location *location;
    int maxActions;
    int actionsLeft;
    std::vector<Item*> items;
    std::vector<Villager *> guideVillagers;

public:
    Hero(const std::string &, Location *, int);
    virtual void move(Location *, const std::vector<Villager *> &);
    virtual void pickUp(const std::vector<Item*> &);
    virtual void guide(Villager *, Location*);
    virtual void advanced(Monster *);
    virtual void defeat(Monster *);
    virtual void specialAction(const std::vector<Item*> &) = 0;
    void resetActions();
    void useAction();
    bool hasActionsLeft() const;
    bool hasRequiredItems(const std::vector<Item> &) const;
    std::string getName() const;
    Location *getLocation() const;
    int getActionsLeft() const;
    std::vector<Item*> getItems() const;
    std::vector<Villager *> getGuideVillagers() const;
    void consumeItems(const std::vector<Item> &);
    void updateGuideVillagers();
    void setLocation(Location *);
};

#endif