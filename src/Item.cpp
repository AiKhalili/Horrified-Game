#include "Item.hpp"
#include <iostream>
using namespace std;

Item::Item(int strength, Color color, std::string name, Location* location){
    this->strength = strength;
    this->color = color;
    this->name = name;
    this->location = location;
}
int Item::get_strength()const{return strength;}
Color Item::get_color()const{return color;}
std::string Item::get_name()const{return name;}
Location* Item:: get_location()const{return location;}
void Item::set_location(Location* location){this->location = location;}