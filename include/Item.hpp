#ifndef ITEM_HPP
#define ITEM_HPP
#include <iostream>

class Location;

enum class Color{red, blue, yellow};

class Item{
    private:
    int strength; //قدرت
    Color color; // سه رنگ ابی قرمز زرد
    std::string name; // اسمش
    Location* location; // مکانی که باید قرارش بدیم

    public:
    Item(int, Color, std::string, Location*);
    int get_strength()const;
    Color get_color()const;
    std::string get_name()const;
    Location* get_location()const;
    void set_location(Location*);
    
};
#endif