#include "core/Villager.hpp"

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