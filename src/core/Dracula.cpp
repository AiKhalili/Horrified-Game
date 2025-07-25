#include "core/Dracula.hpp"
#include "core/Location.hpp"
#include "core/Map.hpp"
#include "core/Hero.hpp"
#include "core/ItemManager.hpp"
#include <iostream>
using namespace std;

Dracula::Dracula() : Monster("Dracula")
{
    setFrenzyOrder(1);
    coffins = {{"Cave", false}, {"Crypt", false}, {"Dungeon", false}, {"Graveyard", false}};
    Map::get_instanse()->addMonsterTo("Cave", this);
}

vector<Location *> Dracula::getAdvanceLocation()
{
    vector<Location *> vecloc;
    for (const auto &pair : coffins)
    {
        const string &locName = pair.first;
        vecloc.push_back(Map::get_instanse()->getLocation(locName));
    }
    return vecloc; // برگردوندن مکان تابوت ها
}

bool Dracula::isAdvanceLocation(const string &) const
{
}

void Dracula::advanceMission(Location *l)
{
    coffins[l->get_name()] = true;
}

bool Dracula::canbedefeated() const
{
    for (const auto &pair : coffins)
    {
        if ((pair.second) == false)
        {
            return false; // حتی اگه یه دونه از تابوت ها هم باقی مونده بود که نابود نشده بود فالس برمی گردونه
        }
    }
    return true;
}

void Dracula::specialPower(Hero *hero)
{
    if (this->get_location() != hero->getLocation()) // اگر قبلا در یک مکان نبودند
    {
        hero->getLocation()->removeHero(hero);
        hero->setLocation(this->get_location()); // دراکولا قهرمان به سمت خودش می کشه 
        hero->getLocation()->addHero(hero);
    }
}

vector<Item> Dracula::getAdvanceRequirement() const
{

    vector<Item> requirements;
    requirements.push_back(Item(6, COlOR::red, "AnyRedItemSum6", nullptr));

    return requirements;
}

vector<Item> Dracula::getDefeatRequirement() const
{

    return {Item(6, COlOR::yellow, "DefeatDracula", nullptr)};
}

int Dracula::getCounter() const
{
    int count = 0;
    for (const auto &pair : coffins)
    {
        if ((pair.second) == true)
        {
            count++;
        }
    }
    return count; // تعداد تابوت های نابود شده
}