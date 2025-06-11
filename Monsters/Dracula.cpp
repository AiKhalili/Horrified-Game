#include "Dracula.hpp"
#include "Location.hpp"
#include "Map.hpp"
#include "Hero.hpp"
#include "ItemManager.hpp"
#include <iostream>
using namespace std;

Dracula::Dracula() : Monster("Dracula") {
    coffins = { {"Cave", false}, {"Crypt", false}, {"Dungeon", false}, {"Graveyard", false} };
    Map::get_instanse()->addMonsterTo("Cave", this);
}

vector<Location*> Dracula::getAdvanceLocation() {
    vector<Location*> vecloc;
    for (const auto& pair : coffins) {
        const string& locName = pair.first;
        vecloc.push_back(Map::get_instanse()->getLocation(locName));
    }
    return vecloc;
}

void Dracula::advanceMission( Location* l) {
    int sum = 0;
    if(coffins[l->get_name()] == false){
        for (const auto& item : l->get_items()){
            if (item->get_color() == Color::red){
                sum += item->get_strength();
            }
        }
    }
    else{
        throw GameException("This coffin has been destroyed!\n");
    }
    if (sum >= 6 ) {
        coffins[l->get_name()] = true;
        for(const auto& item : l->get_items()){
            ItemManager::getInstance().recycleItem(item);
        }
        for(const auto& item : l->get_items()){
            l->removeItem(item);
        }
    }
    else{
        throw GameException("There are no conditions of coffin destruction!\n");
    }
    
}

bool Dracula::canbedefeated() const {
    for (const auto& pair : coffins){
        if ((pair.second)==false){ 
            return false;
        }
    }
    return true;
}

void Dracula::defeat( Location* l) {
    int sum = 0;
    for (const auto& item : l->get_items()){
        if (item->get_color() == Color::yellow){
            sum += item->get_strength();
        }
    }
    if (sum >= 6 && canbedefeated() == true) {
        set_defeated(true);
        for (const auto& item : l->get_items()){
            ItemManager::getInstance().recycleItem(item);
        }
        for (const auto& item : l->get_items()){
            l->removeItem(item);
        }
    }
    else{
        throw GameException("There are no conditions for Dracula's destruction!\n");
    }
}

void Dracula::DarkCharm(Hero* hero){
    if(this->get_location() != hero->getLocation()){
        hero->setLocation(this->get_location());
    }
}

vector<Item> Dracula::getAdvanceRequirement() const{

    std::vector<Item> requirements;
    requirements.push_back(Item(6, Color::red, "AnyRedItemSum6", nullptr));

    return requirements;
}

vector<Item> Dracula::getDefeatRequirement() const {

    return {Item(6, Color::yellow, "DefeatToken", nullptr)};
}

int Dracula::getCounter()const{
    int count = 0;
    for (const auto& pair : coffins){
        if ((pair.second)==true){ 
            count++;
        }
    }
    return count;
}