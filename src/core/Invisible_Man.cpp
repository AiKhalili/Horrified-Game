#include "core/Invisible_Man.hpp"
#include "core/Location.hpp"
#include "core/Map.hpp"
#include "core/Villager.hpp"
#include "core/ItemManager.hpp"
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

    if (villagers.empty()) {
        cout << "No villagers on the map.\n";
        return;
    }
    // بررسی اینکه آیا الان با یک ویلیجر در یک مکان هست
    for (Villager* v : villagers)
    {
        if (v->isAlive() && v->getCurrentLocation() == currentLoc)
        {
            cout << "Found villager '" << v->getName() << "' at same location.\n";
            v->kill();
            cout << v->getName() << " was killed by Invisible Man (Power effect).\n";
            currentLoc->removeVillager(v);
            v->setLocation(nullptr);
            return; // چون کشته شد، دیگه حرکت نمی‌کنیم
        }
    }

    //  پیدا کردن نزدیک‌ترین ویلیجر زنده برای حرکت
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

    //حرکت جداکثر دو قدم
    int steps = std::min(2, (int)shortestPath.size() - 1);
    Location* newLoc = (steps > 0) ? shortestPath[steps] : currentLoc;

    // به‌روزرسانی مکان
    currentLoc->removeMonster(this);
    this->set_location(newLoc);
    newLoc->addMonster(this);

    // بررسی رسیدن بعد از حرکت (بدون kill)
    if (newLoc == closest->getCurrentLocation()) {
        cout << "Invisible Man reached the villager '" << closest->getName() << "' but did not kill (arrived after movement).\n";
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

string Invisible_Man::serialize() const {
    string data = "Invisible_Man|";
    data += (get_location() ? get_location()->get_name() : "null") + "|";
    data += is_defeated() ? "1|" : "0|";

    for (const auto& pair : evidence) {
        data += pair.first + ":" + (pair.second ? "1" : "0") + ",";
    }

    return data;
}

void Invisible_Man::setEvidence(const std::map<std::string, bool>& newEvidence) {
    evidence = newEvidence;
}

