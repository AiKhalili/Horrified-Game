#include "core/Item.hpp"
#include "core/Location.hpp"
#include "core/Map.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
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

void Item::set_strength(int st){
    strength = st;
}

string Item::get_pickedFrom() const
{
    return pickedFrom;
}

string Item::get_color_to_string() const
{
    if (color == COlOR::red)
    {
        return "Red";
    }
    else if (color == COlOR::blue)
    {
        return "Blue";
    }
    else
    {
        return "Yellow";
    }
}

COlOR Item::stringToColor(const std::string& s) {
    if (s == "Red") return COlOR::red;
    if (s == "Blue") return COlOR::blue;
    if (s == "Yellow") return COlOR::yellow;
    throw GameException("Invalid color string: " + s);
}

string Item::serialize() const
{
    string data = "Item|";
    data += name + "|";
    data += to_string(strength) + "|";
    data += get_color_to_string() + "|";
    data += (pickedFrom.empty() ? "null" : pickedFrom);

    return data;
}

Item* Item::deserialize(const std::string& line)
{
    stringstream ss(line);
    string part;

    getline(ss, part, '|'); // "Item"
    getline(ss, part, '|');
    string name = part;

    getline(ss, part, '|');
    int strength = stoi(part);

    getline(ss, part, '|');
    COlOR color;
    if (part == "Red") color = COlOR::red;
    else if (part == "Blue") color = COlOR::blue;
    else if (part == "Yellow") color = COlOR::yellow;
    else throw GameException("Invalid item color: " + part);

    getline(ss, part, '|');
    Location* location = (part == "null") ? nullptr : Map::get_instanse()->getLocation(part);

    Item* item = new Item(strength, color, name, location);
    item->set_pickedFrom(part);

    return item;
}
