#include "core/Archaeologist.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

Archaeologist::Archaeologist(const std::string &name, Map *map)
    : Hero(name, map ? map->getLocation("Docks") : throw GameException("Map is null!"), 4) {}

void Archaeologist::specialAction(const vector<Item *> &neighborItems)
{
    if (!hasActionsLeft())
    {
        throw GameException("No actions left to perform special action!\n");
    }

    int picked = 0;

    for (Item *item : neighborItems)
    {
        if (!item)
        {
            continue;
        }
        Location *itemLocation = item->get_location();
        if (itemLocation && location->isNeighbor(itemLocation))
        {
            items.push_back(item);
            itemLocation->removeItem(item);
            picked++;
        }
    }
    if (picked > 0)
    {
        useAction();
        cout << "Special action: Picked " + to_string(picked) + " nearby item(s)\n";
    }
    else
    {
        cout << "No valid items in adjacent locations!\n";
    }
}

string Archaeologist::getClassName() const
{
    return "Archaeologist";
}