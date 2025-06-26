#ifndef DRACULA_HPP
#define DRACULA_HPP
#include "Monster.hpp"
#include "GameException.hpp"
#include <iostream>
#include <new>
#include <map>
#include <vector>

class Location;
class Hero;

class Dracula : public Monster
{
private:
    std::map<std::string, bool> coffins; // مپ برای تابوت های دراکولا

public:
    Dracula();
    virtual std::vector<Location *> getAdvanceLocation() override;
    virtual void advanceMission(Location *) override;
    virtual bool canbedefeated() const override;
    void specialPower(Hero*) override; // استفاده از قدرت برای هیولا
    virtual std::vector<Item> getAdvanceRequirement() const override;
    virtual std::vector<Item> getDefeatRequirement() const override;
    virtual int getCounter() const override;
    virtual bool isAdvanceLocation(const std::string&) const override;
};

#endif