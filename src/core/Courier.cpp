#include "core/Courier.hpp"
#include "core/Game.hpp"
#include "core/Location.hpp"
using namespace std;

Courier::Courier(const string &name, Map *map)
    : Hero(name, map ? map->getLocation("Shop") : throw GameException("Map is null!"), 4) {}

void Courier::specialAction(const vector<Item *> &)
{
    vector<Hero *> allHeroes = Game::getInstance().getAllHeroes();
    Hero *targetHero = nullptr;

    for (Hero *h : allHeroes)
    {
        if (h->getClassName() != "Courier")
        {
            targetHero = h;
            break;
        }
    }

    if (!targetHero)
    {
        throw GameException("No other hero is available to move to its location!\n");
    }
    if (!targetHero->getLocation())
    {
        throw GameException("Target hero has invalid location.");
    }

    location->removeHero(this);
    targetHero->getLocation()->addHero(this);

    cout << name << " used special action to moved to " << targetHero->getName() << '\n';
}

string Courier::getClassName() const
{
    return "Courier";
}