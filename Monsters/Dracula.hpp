#ifndef DRACULA_HPP
#define DRACULA_HPP
#include "Monster.hpp"
#include "GameException.hpp"
#include <iostream>
#include <new>
#include <map>

class Location;
class Hero;

class Dracula:public Monster{
    private:
    std::map<std::string, bool> coffins;

    public:
    Dracula();
    virtual std::vector<Location*> getAdvanceLocation() override;
    virtual void advanceMission(Location*) override; //ایتم هایی که با یکی از تابوت ها یه جا قرار داره رو میفرستیم
    virtual bool canbedefeated()const override;
    virtual void defeat(Location*) override; //ایتم هایی که با یکی از تابوت ها یه جا قرار داره رو میفرستیم
    void DarkCharm(Hero*);
    virtual std::vector<Item> getAdvanceRequirement() const override;
    virtual vector<Item> getDefeatRequirement() const override;
    virtual int getCounter() const override;
};

#endif