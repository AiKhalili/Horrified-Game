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
    Location* currentLoc = this->get_location();
    vector<Villager*> villagers = Map::get_instanse()->getAllVillagers();

    if (villagers.empty()) return;

    // ------------------------------------------
    // مرحله ۱: بررسی اینکه قبل از حرکت، مرد نامرئی با محلی (Villager) هم‌مکان است یا نه
    // اگر هم‌مکان هستند، ویلیجر کشته می‌شود و حرکت انجام نمی‌شود
    // ------------------------------------------
    for (Villager* v : villagers)
    {
        if (v->isAlive() && v->getCurrentLocation() == currentLoc)
        {
            v->kill();
            cout << v->getName() << " was killed by Invisible Man (Power effect)!\n";
            currentLoc->removeVillager(v);
            v->setLocation(nullptr);
            return; // بلافاصله بعد از کشتن، تابع تمام می‌شود و حرکت انجام نمی‌شود
        }
    }

    // ------------------------------------------
    // مرحله ۲: پیدا کردن نزدیک‌ترین ویلیجر زنده برای حرکت به سمت او
    // حرکت حداکثر دو قدم انجام می‌شود
    // ------------------------------------------
    Villager* closest = nullptr;
    vector<Location*> shortestPath;
    size_t minDist = INT_MAX;

    for (Villager* v : villagers)
    {
        if (!v->isAlive()) continue;

        vector<Location*> path = findShortestPath(currentLoc, v->getCurrentLocation());
        if (!path.empty() && path.size() < minDist)
        {
            minDist = path.size();
            shortestPath = path;
            closest = v;
        }
    }

    if (!closest || shortestPath.empty()) return;

    int steps = std::min(2, (int)shortestPath.size() - 1);
    Location* newLoc = (steps > 0) ? shortestPath[steps] : currentLoc;

    // ------------------------------------------
    // مرحله ۳: جابه‌جایی مرد نامرئی به مکان جدید
    // ------------------------------------------
    currentLoc->removeMonster(this);
    this->set_location(newLoc);
    newLoc->addMonster(this);
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