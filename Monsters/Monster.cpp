#include "Monster.hpp"
#include "Location.hpp"
#include "Hero.hpp"
#include "Villager.hpp"
#include <iostream>
using namespace std;

Monster::Monster(std::string name){
    this->name = name;
    this->location = location;
    this->defeated = false;
}
string Monster::get_name()const{return name;}
Location* Monster::get_location()const{return location;}
bool Monster::is_defeated()const{return defeated;}

void Monster::set_location(Location* location){
    this->location = location;
}

void Monster::set_defeated(bool defeated){
    this->defeated = defeated;
}
// تابع کمکی پیدا کردن مسیر کوتاه‌ترین مسیر بین دو Location
std::vector<Location*> Monster::findShortestPath(Location* start, Location* goal) {
    std::queue<Location*> q;
    std::unordered_map<Location*, Location*> parent;
    std::vector<Location*> path;

    q.push(start);
    parent[start] = nullptr;

    while (!q.empty()) {
        Location* current = q.front();
        q.pop();

        if (current == goal) {
            for (Location* at = goal; at != nullptr; at = parent[at]) {
                path.push_back(at);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (Location* neighbor : current->get_neighbors()) {
            if (neighbor && parent.find(neighbor) == parent.end()) {
                parent[neighbor] = current;
                q.push(neighbor);
            }
        }
    }
    return path; // مسیر خالی اگر پیدا نشد
}

#include <queue>
#include <climits>
#include <algorithm>

MoveResult Monster::moveTowardTarget(std::vector<Hero*> heroes, std::vector<Villager*> villagers, int maxSteps) {
    Location* currentLocation = this->get_location();

    // 1. اگه هیولا الان روی قهرمان باشه، حرکت نمی‌کنه و همونجا می‌مونه
    for (Hero* h : heroes) {
        if (h->getLocation() == currentLocation) {
            return MoveResult::ReachedHero;
        }
    }

    // 2. یا روی روستایی باشه
    for (Villager* v : villagers) {
        if (v->getCurrentLocation() == currentLocation) {
            return MoveResult::ReachedVillager;
        }
    }

    // 3. پیدا کردن نزدیک‌ترین هدف با اولویت قهرمان‌ها
    Location* targetLocation = nullptr;
    std::vector<Location*> shortestPath;
    int minDistance = INT_MAX;
    bool foundHero = false;

    for (Hero* h : heroes) {
        std::vector<Location*> path = findShortestPath(currentLocation, h->getLocation());
        if (!path.empty()) {
            int dist = path.size();
            if (dist < minDistance || (dist == minDistance && !foundHero)) {
                minDistance = dist;
                shortestPath = path;
                targetLocation = h->getLocation();
                foundHero = true;
            }
        }
    }

    if (!foundHero) {
        for (Villager* v : villagers) {
            std::vector<Location*> path = findShortestPath(currentLocation, v->getCurrentLocation());
            if (!path.empty() && path.size() < minDistance) {
                minDistance = path.size();
                shortestPath = path;
                targetLocation = v->getCurrentLocation();
            }
        }
    }

    // 4. اگر هدفی پیدا نشد یا هیولا نمی‌تونه حرکت کنه (راهی نیست یا هدف همینجاست)
    if (shortestPath.empty() || shortestPath.size() <= 1) {
        return MoveResult::NoTarget;
    }

    // 5. حرکت هیولا تا حد مجاز
    int stepsToMove = std::min(maxSteps, (int)shortestPath.size() - 1);
    Location* newLocation = shortestPath[stepsToMove];

    currentLocation->removeMonster(this);
    newLocation->addMonster(this);

    // 6. بررسی وضعیت پس از حرکت: آیا رسید به قهرمان یا روستایی؟
    for (Hero* h : heroes) {
        if (h->getLocation() == newLocation) {
            return MoveResult::ReachedHero;
        }
    }

    for (Villager* v : villagers) {
        if (v->getCurrentLocation()== newLocation) {
            return MoveResult::ReachedVillager;
        }
    }

    // 7. هیولا حرکت کرده ولی به هدف نرسیده
    return MoveResult::MovedOnly;
}
