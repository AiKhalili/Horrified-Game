#include "core/Monster.hpp"
#include "core/Location.hpp"
#include "core/Hero.hpp"
#include "core/Villager.hpp"
#include <iostream>
using namespace std;

Monster::Monster(std::string name)
{
    this->name = name;
    this->defeated = false;
}
string Monster::get_name() const { return name; }
Location *Monster::get_location() const { return location; }
bool Monster::is_defeated() const { return defeated; }

void Monster::set_location(Location *location)
{
    this->location = location;
}

void Monster::set_defeated(bool defeated)
{
    this->defeated = defeated;
}
// تابع کمکی پیدا کردن مسیر کوتاه‌ترین مسیر بین دو Location
vector<Location *> Monster::findShortestPath(Location *start, Location *goal)
{
    queue<Location *> q;
    unordered_map<Location *, Location *> parent;
    vector<Location *> path;

    q.push(start);
    parent[start] = nullptr;

    while (!q.empty())
    {
        Location *current = q.front();
        q.pop();

        if (current == goal)
        {
            for (Location *at = goal; at != nullptr; at = parent[at])
            {
                path.push_back(at);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (Location *neighbor : current->get_neighbors())
        {
            if (neighbor && parent.find(neighbor) == parent.end())
            {
                parent[neighbor] = current;
                q.push(neighbor);
            }
        }
    }
    return path; // مسیر خالی اگر پیدا نشد
}

Location *Monster::moveTowardTarget(vector<Hero *> heroes, vector<Villager *> villagers, int maxSteps)
{
    Location *currentLocation = this->get_location();

    // 1. اگه هیولا الان روی قهرمان یا روستایی باشه، حرکت نمی‌کنه
    for (Hero *h : heroes)
    {
        if (h->getLocation() == currentLocation)
        {
            return currentLocation;
        }
    }

    for (Villager *v : villagers)
    {
        if (v->getCurrentLocation() == currentLocation)
        {
            return currentLocation;
        }
    }

    // 2. پیدا کردن نزدیک‌ترین هدف
    Location *heroTarget = nullptr;
    Location *villagerTarget = nullptr;
    vector<Location *> heroPath;
    vector<Location *> villagerPath;

    size_t minHeroDistance = INT_MAX;
    size_t minVillagerDistance = INT_MAX;

    // پیدا کردن مسیر به نزدیک‌ترین قهرمان
    for (Hero *h : heroes)
    {
        vector<Location *> path = findShortestPath(currentLocation, h->getLocation());
        if (!path.empty() && path.size() < minHeroDistance)
        {
            minHeroDistance = path.size();
            heroPath = path;
            heroTarget = h->getLocation();
        }
    }

    // پیدا کردن مسیر به نزدیک‌ترین روستایی
    for (Villager *v : villagers)
    {
        vector<Location *> path = findShortestPath(currentLocation, v->getCurrentLocation());
        if (!path.empty() && path.size() < minVillagerDistance)
        {
            minVillagerDistance = path.size();
            villagerPath = path;
            villagerTarget = v->getCurrentLocation();
        }
    }

    // 3. مقایسه‌ی فاصله‌ها و تصمیم برای حرکت
    vector<Location *> chosenPath;
    Location *finalTarget = nullptr;

    if (minHeroDistance < minVillagerDistance)
    {
        chosenPath = heroPath;
        finalTarget = heroTarget;
    }
    else if (minVillagerDistance < minHeroDistance)
    {
        chosenPath = villagerPath;
        finalTarget = villagerTarget;
    }
    else if (minHeroDistance != INT_MAX)
    { // فاصله‌ها برابرن و مسیر هست
        // اولویت با قهرمان
        chosenPath = heroPath;
        finalTarget = heroTarget;
    }
    else
    {
        // نه قهرمانی هست، نه روستایی‌ای که بهش برسیم
        return nullptr;
    }

    // 4. حرکت به سمت هدف
    if ((int)chosenPath.size() <= maxSteps + 1)
    {
        // می‌رسه به هدف
        Location *destination = finalTarget;

        this->get_location()->removeMonster(this);
        this->set_location(destination);
        destination->addMonster(this);

        return destination;
    }
    else
    {
        // نمی‌رسه، فقط تا حداکثر استپ جلو می‌ره
        Location *intermediate = chosenPath[maxSteps];

        this->get_location()->removeMonster(this);
        this->set_location(intermediate);
        intermediate->addMonster(this);

        return nullptr;
    }
}

void Monster::setFrenzyOrder(int order){
    frenzyOrder = order;
}

int Monster::getFrenzyOrder() const{
    return frenzyOrder;
}