#include "core/Villager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/Map.hpp"

using namespace std;

Villager::Villager(const std::string &name, Location *start, Location *safeLocation)
    : name(name),
      currentLocation(start),
      safeLocation(safeLocation),
      alive(true),
      rescue(false) {}

string Villager::getName() const
{
    return name;
}

Location *Villager::getCurrentLocation() const
{
    return currentLocation;
}

Location *Villager::getSafeLocation() const
{
    return safeLocation;
}

void Villager::setLocation(Location *newLocation)
{
    currentLocation = newLocation;
}

bool Villager::isSafeLocation() const
{
    return currentLocation == safeLocation;
}

bool Villager::isAlive() const
{
    return alive;
}

void Villager::kill()
{
    alive = false;
}

void Villager::markRescued()
{
    rescue = true;
    alive = false;
}

bool Villager::isRescued() const
{
    return rescue;
}

string Villager::serialize() const
{
    string data = "Villager|";
    data += name + "|";
    data += (currentLocation ? currentLocation->get_name() : "null") + "|";
    data += (safeLocation ? safeLocation->get_name() : "null") + "|";
    data += (alive ? "1|" : "0|");
    data += (rescue ? "1" : "0");


    return data;
}

Villager* Villager::deserialize(const std::string& line)
{
    stringstream ss(line);
    string part;

    getline(ss, part, '|'); // "Villager"
    getline(ss, part, '|');
    string name = part;

    getline(ss, part, '|');
    Location* current = (part == "null") ? nullptr : Map::get_instanse()->getLocation(part);

    getline(ss, part, '|');
    Location* safe = (part == "null") ? nullptr : Map::get_instanse()->getLocation(part);

    getline(ss, part, '|');
    bool alive = (part == "1");

    getline(ss, part, '|');
    bool rescued = (part == "1");

    Villager* v = new Villager(name, current, safe);

    if (current != nullptr)
    {
        Map::get_instanse()->addVillager(current->get_name(), v);
    }
    else
    {
        v->setLocation(nullptr);
    }

    if (!alive)
    {
        v->kill();
        v->setLocation(nullptr);
        
        if (current != nullptr)
            Map::get_instanse()->removeVillagerFrom(current->get_name(), v);
    }

    if (rescued)
        v->markRescued();

    return v;
}
