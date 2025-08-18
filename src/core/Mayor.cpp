#include "core/Hero.hpp"
#include "core/Mayor.hpp"
#include "core/Map.hpp"
#include <iostream>

using namespace std;

class Map;

Mayor::Mayor(const std::string &name, Map *map)
    : Hero(name, map ? map->getLocation("Theatre") : throw GameException("Map is null!"), 5) {}

void Mayor::specialAction(const vector<Item *> &)
{
    throw GameException("Mayor has no special ability!\n");
}

string Mayor::getClassName() const
{
    return "Mayor";
}