#include "Invisible_Man.hpp"
#include "Location.hpp"
#include "Map.hpp"
#include "Villager.hpp"
#include "ItemManager.hpp"
#include <iostream>
using namespace std;

Invisible_Man::Invisible_Man() : Monster("Invisible Man")
{
    setFrenzyOrder(6);
    evidence = {{"Inn", false}, {"Barn", false}, {"Laboratory", false}, {"Institute", false}, {"Mansion", false}};
    Map::get_instanse()->addMonsterTo("Inn", this);

    advanceTracker = {{"Inn", false}, {"Barn", false}, {"Institute", false}, {"Laboratory", false}, {"Mansion", false}};
}

bool Invisible_Man::isAdvanceLocation(const std::string &location) const
{
    return advanceTracker.count(location) > 0; // بررسی می‌کند که آیا یک مکان خاص جزء مکان‌هایی است که باید در آن مدرک پیدا شود یا نه
}

vector<Location *> Invisible_Man::getAdvanceLocation()
{
    return {Map::get_instanse()->getLocation("Precinct")};
}

void Invisible_Man::advanceMission(Location *l)
{
    if (!isAdvanceLocation(l->get_name())) // اگر جزو اون مکان ها بود
    {
        throw GameException("This is not a valid location to advance the Invisible Man mission.\n");
    }

    if (evidence[l->get_name()]) // اگر قبلا مدرک این مکان برداشته شده بود
    {
        throw GameException("Evidence already found at this location!\n");
    }

    // Mark as complete
    evidence[l->get_name()] = true;
}

bool Invisible_Man::canbedefeated() const
{
    for (const auto &pair : evidence)
    {
        if ((pair.second) == false)
        {
            return false;
        }
    }
    return true;
}

void Invisible_Man::specialPower(Hero *)
{
    Villager *dummyVillager;
    Location *currentLoc = this->get_location();
    vector<Villager *> villagers = Map::get_instanse()->getAllVillagers();

    if (villagers.empty())
        return;

    Location *targetLoc = nullptr;
    vector<Location *> shortestPath;
    size_t minDist = INT_MAX;

    // پیدا کردن نزدیک‌ترین Villager
    for (Villager *v : villagers)
    {
        if (!v->isAlive())
        {
            continue;
        }
        vector<Location *> path = findShortestPath(currentLoc, v->getCurrentLocation());
        if (!path.empty() && path.size() < minDist)
        {
            minDist = path.size();
            shortestPath = path;
            targetLoc = v->getCurrentLocation();
            dummyVillager = v;
        }
    }

    // اگر Villager نداشت یا بهش راه نبود
    if (targetLoc == nullptr || shortestPath.empty())
        return;

    // بررسی اینکه آیا هیولا همین حالا در مکان Villager هست
    bool alreadyThere = (currentLoc == targetLoc);

    // تعداد گام‌هایی که قرار است برداشته شود (حداکثر 2)
    int steps = std::min(2, (int)shortestPath.size() - 1); // چون اولین مکان خودشه

    // پیدا کردن مکان جدید پس از حرکت
    Location *newLocation = (steps > 0) ? shortestPath[steps] : currentLoc;

    // به‌روزرسانی موقعیت هیولا
    currentLoc->removeMonster(this);
    this->set_location(newLocation);
    newLocation->addMonster(this);

    // اگر به Villager رسیدیم و قبلاً در آن مکان بودیم، او را بکشیم
    if (newLocation == dummyVillager->getCurrentLocation() && alreadyThere)
    {
        dummyVillager->kill();
    }
}

vector<Item> Invisible_Man::getAdvanceRequirement() const
{
    vector<Item> result;

    for (const auto &pair : advanceTracker)
    {
        if (!pair.second)
        {
            Location *loc = Map::get_instanse()->getLocation(pair.first);
            if (!loc)
                cout << "Location not found in map for advance requirement: " << pair.first << endl;

            result.push_back(Item(0, COlOR::red, "AdvanceToken", loc));
        }
    }
    return result;
}

vector<Item> Invisible_Man::getDefeatRequirement() const
{

    return {Item(9, COlOR::red, "DefeatInvisible", nullptr)};
}

int Invisible_Man::getCounter() const
{
    int count = 0;
    for (const auto &pair : evidence)
    {
        if ((pair.second) == true)
        {
            count++;
        }
    }
    return count;
}