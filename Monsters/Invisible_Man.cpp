#include "Invisible_Man.hpp"
#include "Location.hpp"
#include "Map.hpp"
#include "Villager.hpp"
#include "ItemManager.hpp"
#include <iostream>
using namespace std;

Invisible_Man::Invisible_Man() : Monster("Invisible Man") {
        evidence = { {"Inn", false}, {"Barn", false}, {"Laboratory", false}, {"Institute", false}, {"Mansion", false} };
        Map::get_instanse()->addMonsterTo("Inn", this);

        advanceTracker = {{"Inn", false},{"Barn", false},{"Institute", false},{"Laboratory", false},{"Mansion", false}};
}

vector<Location*> Invisible_Man::getAdvanceLocation(){
    vector<Location*> vecloc;
    for (const auto& pair : evidence) {
        const string& locName = pair.first;
        vecloc.push_back(Map::get_instanse()->getLocation(locName));
    }
    return vecloc;
}

void Invisible_Man::advanceMission(Location* l) {

    if (((l->get_items()).size() != 0 ) && evidence[l->get_name()] == false){
        evidence[l->get_name()] = true;
    }
    else if(l->get_items().size() == 0){
        throw GameException("There is no item in this location where we can find evidence of the Invisible Man's existence!\n");
    }
    else if(evidence[l->get_name()] != false){
        throw GameException("Evidence item proving the existence of the Invisible Man has been found at this location before!\n");
    }
    for (const auto& item : l->get_items()){
        ItemManager::getInstance().recycleItem(item);
        l->removeItem(item);
    }
    
}

bool Invisible_Man::canbedefeated() const {
    for (const auto& pair : evidence){
        if ((pair.second)==false){ 
            return false;
        }
    }
        return true;
}

void Invisible_Man::defeat(Location* l) {
    int sum = 0;
    for (const auto& item : l->get_items()){
        if (item->get_color() == Color::red){
            sum += item->get_strength();
        }
    }
    if (sum >= 9 && canbedefeated() == true) {
        set_defeated(true);
        for (const auto& item : l->get_items()){
            ItemManager::getInstance().recycleItem(item);
            l->removeItem(item);
        }
    }
    else{
        throw GameException("There are no conditions for InvisibleMan's destruction!\n");
    }
}

void Invisible_Man::StalkUnseen(Location* Newloc, Villager* villager){

    if(this->get_location() == villager->getCurrentLocation()){
        villager->kill();
    }
    else{
        this->set_location(Newloc);
    }
}

vector<Item> Invisible_Man::getAdvanceRequirement() const {
    vector<Item> result;

    for (const auto& pair : advanceTracker) {
        if (!pair.second) {
            result.push_back(Item(0, Color::red, "AdvanceToken", Map::get_instanse()->getLocation(pair.first)));
        }
    }

    return result;
}

void Invisible_Man::markAdvanceComplete(const string& locationName) {
    if (advanceTracker.count(locationName)) {
        advanceTracker[locationName] = true;
    }
}

bool Invisible_Man::isAdvanceLocation(const string& location) const {
    return advanceTracker.count(location) > 0;
}

bool Invisible_Man::isAdvanceCompleted(const string& location) const {
    auto it = advanceTracker.find(location);
    return it != advanceTracker.end() && it->second;
}

vector<Item> Invisible_Man::getDefeatRequirement() const {

     return {Item(9, Color::red, "DefeatToken", nullptr)};
}

int Invisible_Man::getCounter()const{
    int count = 0;
    for (const auto& pair : evidence){
        if ((pair.second)==true){ 
            count++;
        }
    }
    return count;
}