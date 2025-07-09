#include "core/Item.hpp"
#include "core/Location.hpp"
#include <iostream>
using namespace std;

Item::Item(int strength, COlOR color, string name, Location *location)
{
    this->strength = strength;
    this->color = color;
    this->name = name;
    this->location = location;
    if (location)
    {
        this->pickedFrom = location->get_name();
    }
}

int Item::get_strength() const { return strength; }
COlOR Item::get_color() const { return color; }
string Item::get_name() const { return name; }
Location *Item::get_location() const { return location; }
void Item::set_location(Location *location) { this->location = location; }

void Item::set_pickedFrom(const string &loc)
{
    pickedFrom = loc;
}

string Item::get_pickedFrom() const
{
    return pickedFrom;
}

string Item::get_color_to_string()
{
    if (color == COlOR::red)
    {
        return "red";
    }
    else if (color == COlOR::blue)
    {
        return "blue";
    }
    else
    {
        return "yellow";
    }
}