#ifndef ITEM_HPP
#define ITEM_HPP
#include <iostream>

class Location;

enum class COlOR
{
    red,
    blue,
    yellow
};

class Item
{
private:
    int strength;
    COlOR color;
    std::string name;
    Location *location;     // مکانی که باید قرارش بدیم
    std::string pickedFrom; // آیتم از کجا برداشته شده

public:
    Item(int, COlOR, std::string, Location *);
    int get_strength() const;
    COlOR get_color() const;
    std::string get_color_to_string();
    std::string get_name() const;
    Location *get_location() const;
    std::string get_pickedFrom() const;
    void set_location(Location *);
    void set_pickedFrom(const std::string &);
};
#endif