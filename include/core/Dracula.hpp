#ifndef DRACULA_HPP
#define DRACULA_HPP
#include "core/Monster.hpp"
#include "core/GameException.hpp"
#include <iostream>
#include <new>
#include <map>
#include <vector>

class Location;
class Hero;

class Dracula : public Monster
{
private:
    std::map<std::string, bool> coffins;

public:
    Dracula();
    virtual std::vector<Location *> getAdvanceLocation() override;
    virtual void advanceMission(Location *) override;
    virtual bool canbedefeated() const override;
    PowerResult specialPower(Hero *) override;
    virtual std::vector<Item> getAdvanceRequirement() const override;
    virtual std::vector<Item> getDefeatRequirement() const override;
    virtual int getCounter() const override;
    virtual bool isAdvanceLocation(const std::string &) const override;
    virtual std::string serialize() const override;
    void setCoffins(const std::map<std::string, bool> &newCoffins);

    bool isCoffinLocation(const std::string &locName) const;
    bool isCoffinDestroyed(const std::string &locNamw) const;
};

#endif