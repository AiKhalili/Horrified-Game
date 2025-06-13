#include "Mayor.hpp"
#include <iostream>

using namespace std;

Mayor::Mayor(const std::string &name, Map *map)
    : Hero(name, map->getLocation("Theater"), 4) {}

void Mayor::specialAction(const vector<Item *> &)
{
    tui.showMessageScreen("Mayor has no special ability!");
}

string Mayor::getClassName() const
{
    return "Mayor";
}