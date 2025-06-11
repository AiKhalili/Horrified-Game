#ifndef INVISIBLE_MAN_HPP
#define INVISIBLE_MAN_HPP
#include "Monster.hpp"
#include "GameException.hpp"
#include <iostream>
#include <new>
#include <map>

class Location;
class Villager;

class Invisible_Man:public Monster{
    private:
    std::map<std::string, bool> evidence;
    std::map<std::string, bool> advanceTracker;

    public:
    Invisible_Man();
    virtual std::vector<Location*> getAdvanceLocation() override;
    virtual void advanceMission(Location*) override; 
    virtual bool canbedefeated()const override;
    virtual void defeat(Location*) override;
    void StalkUnseen(Location*, Villager*);
    virtual std::vector<Item> getAdvanceRequirement() const override;
    virtual void markAdvanceComplete(const std::string&) override;
    bool Invisible_Man::isAdvanceLocation(const std::string&) const;
    bool Invisible_Man::isAdvanceCompleted(const std::string&) const;
    virtual vector<Item> getDefeatRequirement() const override;
    virtual int getCounter() const override;
};





#endif