#ifndef INVISIBLE_MAN_HPP
#define INVISIBLE_MAN_HPP
#include "core/Monster.hpp"
#include "core/GameException.hpp"
#include <iostream>
#include <vector>
#include <new>
#include <map>

class Location;
class Villager;

class Invisible_Man : public Monster
{
private:
    std::map<std::string, bool> evidence;
    std::map<std::string, bool> advanceTracker; // برای مشخص کردن پیشرفت مأموریت در هر مکان (تکمیل یا ناقص) است

public:
    Invisible_Man();
    virtual std::vector<Location *> getAdvanceLocation() override;
    virtual void advanceMission(Location *) override;
    virtual bool canbedefeated() const override;
    PowerResult specialPower(Hero *) override;
    virtual std::vector<Item> getAdvanceRequirement() const override;
    virtual std::vector<Item> getDefeatRequirement() const override;
    virtual int getCounter() const override;
    virtual bool isAdvanceLocation(const std::string &) const override;
    virtual std::string serialize() const override;
    void setEvidence(const std::map<std::string, bool> &newEvidence);
    virtual Location *moveTowardTarget(std::vector<Hero *> heroes, std::vector<Villager *> villagers, int maxSteps) override;
};

#endif